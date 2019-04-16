#include "XXfmMp4Writer.h"
#include <lib/ffmpeg/include/libavcodec/avcodec.h>
#include <lib/ffmpeg/include/libavformat/avformat.h>
#include <pthread.h>
#include <unistd.h>

#define WRITER_DEBUG                        // 调试信息输出宏
#define MIN_MSEC_TIMESTAMP  31507200000     // 毫秒级别时间戳最小值
#define MAX_MSEC_TIMESTAMP  4687730032000   // 毫秒级别时间戳最大值
#define IS_MSEC(timestamp)  (MIN_MSEC_TIMESTAMP < timestamp && timestamp  < MAX_MSEC_TIMESTAMP)

#define H264_SPS_CODEC      0x07
#define H264_PPS_CODEC      0x08
#define H264_IDR_CODEC      0x05
#define H264_P_CODEC        0x01

#define H265_VPS_CODEC      0x20
#define H265_SPS_CODEC      0x21
#define H265_PPS_CODEC      0x22
#define H265_IDR_CODEC      0x13
#define H265_P_CODEC        0x01

const char HEV1[]           = "hev1";
const char HVC1[]           = "hvc1";
const uint8_t START_CODE[]  = {0x00, 0x00, 0x00, 0x01};

// 句柄实际内容,需要handle->context的强制转换
typedef struct XXfmMp4WriterContext{
    AVFormatContext *context;           //
    bool            isInited;           // 是否已经获得视频关键帧

    AVStream        *videoStream;       // 视频轨
    AVCodecContext  *videoCodecContext; // 视频编码器
    int             videoFrameRate;     // 视频帧率
    int             videoFrameCount;    // 已写入视频帧数
    int64_t         videoFirstTimestamp;
    
    AVStream        *audioStream;       // 音频轨
    AVCodecContext  *audioCodecContext; // 音频编码器
    int             audioSampleRate;    // 音频采样率
    int             audioFrameCount;    // 已写入音频帧数
    int64_t         audioFirstTimestamp;
    
    //AVBSFContext    *videoBSFContext;   // 视频流数据过滤器(暂时视频已经分配,但是没有实际调用)
    AVBSFContext    *audioBSFContext;   // 音频流数据过滤器
}XXfmMp4WriterContext;

// nalu类型
typedef enum XXavNaluType{
    XXavNaluType_Unknown = 1,
    XXavNaluType_VPS,
    XXavNaluType_SPS,
    XXavNaluType_PPS,
    XXavNaluType_IFrame,
    XXavNaluType_PFrame,
}XXavNaluType;

// nalu对象
typedef struct XXavNalu{
    XXavNaluType type;  // Nalu类型
    uint8_t *data;      //
    uint32_t length;    // Nalu的长度
}XXavNalu;

// 私有函数声明
static AVCodecContext* createCodecContext(enum AVCodecID codecID, void *param);             // 创建对应的CodecContext
static AVStream* addStream(AVFormatContext *formatContext, AVCodecContext *codecContext);   // 在formatContext中创建新的音视频轨

static enum AVCodecID videoTypeToAVCodecID(enum XXfmMp4WriterVideoType type);               // 支持的视频类型对应的ffmpeg视频类型
static int makeAvcc(AVCodecContext *context, XXavNalu *sps, XXavNalu *pps);                 // 生成h264对应的视频参数(具体百度avcc结构)
static int makeHvcc(AVCodecContext *context, XXavNalu *sps, XXavNalu *pps, XXavNalu *vps);  // 生成h265对应的视频参数(具体百度hvcc结构)
static int initVideoContext(XXfmMp4WriterContext *context, XXavNalu *vps, XXavNalu *sps, XXavNalu *pps);    // 初始化videoContext

static enum AVCodecID audioTypeToAVCodecID(enum XXfmMp4WriterAudioType type);                       // 支持的音频类型对应的ffmpeg音频类型
static void makeAdts(AVCodecContext *context, int sampleRate, uint8_t channel);                     // 生成AAC的adts部分参数体
static void configAudioCodecContext(AVCodecContext *codecContext, XXfmMp4WriterAudioParam *param);  // 对不同的编码的codecContext做特定配置

static int splitNalu(XXavNalu naluList[], int naluListMax, uint8_t *data, int length, bool isH264);     // 拆机nalu
static XXavNaluType toXXavNaluType(uint8_t *data, bool isH264);                                         // 将h26x的nalu类型转换到本文件定义的枚举
static int replaceStringInFile(const char *filePath, const char *src, const char *dest, int length);    // 文件内字符串替换,src和dest的长度需要一致,都为length

// 创建句柄
XXfmMp4WriterHandle xxfmMp4Writer_create(char *path, XXfmMp4WriterVideoParam *videoParam, XXfmMp4WriterAudioParam *audioParam){
    av_register_all();
    avcodec_register_all();
    
    AVFormatContext *context            = NULL;
    AVCodecContext *videoCodecContext   = NULL;
    AVStream *videoStream               = NULL;
    //AVBSFContext *videoBSFContext       = NULL;
    AVCodecContext *audioCodecContext   = NULL;
    AVStream *audioStream               = NULL;
    AVBSFContext *audioBSFContext       = NULL;
    
    // 创建输出context
    avformat_alloc_output_context2(&context, NULL, NULL, path);
    if (NULL == context) {
        goto FailureToCreate;
    }
    
    // 获取对应的音视频编码ID
    enum AVCodecID videoCodecID = videoTypeToAVCodecID(videoParam->type);
    if (AV_CODEC_ID_NONE == videoCodecID) {
        goto FailureToCreate;
    }
    enum AVCodecID audioCodecID = AV_CODEC_ID_NONE;
    if (NULL != audioParam) {
        audioCodecID = audioTypeToAVCodecID(audioParam->type);
        if (AV_CODEC_ID_NONE == audioCodecID) {
            goto FailureToCreate;
        }
    }
    
    // 打开视频编解码器
    videoCodecContext = createCodecContext(videoCodecID, videoParam);
    if (NULL == videoCodecContext) {
       goto FailureToCreate;
    }
    
    // 创建视频流
    videoStream = addStream(context, videoCodecContext);
    if (NULL == videoStream) {
        goto FailureToCreate;
    }
    
    // 打开音频编解码器和创建音频流
    if (NULL != audioParam) {
        audioCodecContext = createCodecContext(audioCodecID, audioParam);
        if (NULL == audioCodecContext) {
            goto FailureToCreate;
        }
        
        audioStream = addStream(context, audioCodecContext);
        if (NULL == audioStream) {
            goto FailureToCreate;
        }
    }
    
    // 过滤器
    if (NULL != audioParam && XXfmMp4WriterAudioType_AAC == audioParam->type) {
        av_bsf_alloc(av_bsf_get_by_name("aac_adtstoasc"), &audioBSFContext);
    }
    
    // 打开文件
    if (avio_open(&context->pb, path, AVIO_FLAG_WRITE) < 0){
        goto FailureToCreate;
    }
    
    // writer对象
#ifdef WRITER_DEBUG
    printf("[XXfmMp4Writer] succeed to create handle\r\n");
#endif
    XXfmMp4WriterContext *writerContext = (XXfmMp4WriterContext*)malloc(sizeof(XXfmMp4WriterContext));
    writerContext->context              = context;
    writerContext->isInited             = false;

    writerContext->videoCodecContext    = videoCodecContext;
    writerContext->videoStream          = videoStream;
    writerContext->videoFrameRate       = videoParam->frameRate;
    writerContext->videoFrameCount      = 0;
    writerContext->videoFirstTimestamp  = -1;

    if (NULL != audioParam) {
        writerContext->audioCodecContext    = audioCodecContext;
        writerContext->audioStream          = audioStream;
        writerContext->audioSampleRate      = audioParam->sampleRate;
        writerContext->audioFrameCount      = 0;
        writerContext->audioFirstTimestamp  = -1;
    }

    //writerContext->videoBSFContext      = videoBSFContext;
    writerContext->audioBSFContext      = audioBSFContext;
    return writerContext;
    
FailureToCreate:
#ifdef WRITER_DEBUG
    printf("[XXfmMp4Writer] failure to create handle\r\n");
#endif
//    if (videoBSFContext) {
//        av_bsf_free(&videoBSFContext);
//    }
    if (audioBSFContext) {
        av_bsf_free(&audioBSFContext);
    }
    if (videoCodecContext) {
        avcodec_close(videoCodecContext);
        avcodec_free_context(&videoCodecContext);
    }
    if (audioCodecContext) {
        avcodec_close(audioCodecContext);
        avcodec_free_context(&audioCodecContext);
    }
    if (context) {
        avio_close(context->pb);
        avformat_free_context(context);
    }
    return NULL;
}
// 写入视频数据
int xxfmMp4Writer_video(XXfmMp4WriterHandle handle, uint8_t *data, int length, int64_t timestamp, bool isKeyFrame){
    XXfmMp4WriterContext *context   = (XXfmMp4WriterContext*)handle;
    if (NULL == context || (NULL == data && length != 0)) { //} || 0 != waitToLocked(context)) {
        return -1;
    }
    
    // 等待视频关键帧写入
    if (!context->isInited && !isKeyFrame){
        return -1;
    }
    
    AVPacket *packet = av_packet_alloc();
    if (NULL != data && length > 0) {
        // 拆分nalu单元
        XXavNalu naluList[10];
        bool isH264     = context->videoCodecContext->codec_id == AV_CODEC_ID_H264;
        int naluCount   = splitNalu(naluList, 10, data, length, isH264);
        if (naluCount <= 0) {
            av_packet_free(&packet);
            return -1;
        }
        
        // 找出对应的单元
        XXavNalu *vps, *sps, *pps, *iframe, *pframe;
        vps = sps = pps = iframe = pframe = NULL;
        for (int index = 0; index < naluCount; index++) {
            XXavNaluType type = naluList[index].type;
            if (XXavNaluType_VPS == type)           { vps = &(naluList[index]); }
            else if(XXavNaluType_SPS == type)       { sps = &(naluList[index]); }
            else if(XXavNaluType_PPS == type)       { pps = &(naluList[index]); }
            else if(XXavNaluType_IFrame == type)    { iframe = &(naluList[index]); }
            else if(XXavNaluType_PFrame == type)    { pframe = &(naluList[index]); }
            else {}
        }
        
        // 初始化视频参数
        if(!context->isInited){
            // 注意判断的优先级,请谨慎修改
            if(NULL == sps || NULL == pps || (!isH264 && NULL == vps) || initVideoContext(context, vps, sps, pps) < 0){
                av_packet_free(&packet);
                return -1;
            }
            else{
                context->isInited = true;
            }
        }
        
        // 已知当前数据头4字节为annexb的启动头(0x00000001),把帧长度以大端模式覆盖启动头,
        if (isKeyFrame && NULL != iframe) {
            packet->data    = malloc(iframe->length + 4);
            packet->data[0] = (iframe->length) >> 24;
            packet->data[1] = (iframe->length) >> 16;
            packet->data[2] = (iframe->length) >> 8;
            packet->data[3] = (iframe->length);
            packet->size    = iframe->length + 4;
            memcpy(packet->data + 4, iframe->data, iframe->length);
        }
        else if(NULL != pframe){
            packet->data    = malloc(pframe->length + 4);
            packet->data[0] = (pframe->length) >> 24;
            packet->data[1] = (pframe->length) >> 16;
            packet->data[2] = (pframe->length) >> 8;
            packet->data[3] = (pframe->length);
            packet->size    = pframe->length + 4;
            memcpy(packet->data + 4, pframe->data, pframe->length);
        }
        else{
            av_packet_free(&packet);
            return -1;
        }
    }
    
    // 组成AVPacket
    int currentIndex    = context->videoFrameCount++;
    int timeBaseDen     = context->videoStream->time_base.den;
    
    if (length == 0 || NULL == data) {
        packet->size    = 0;
    }
    if (isKeyFrame) {
        packet->flags |= AV_PKT_FLAG_KEY;
    }
    
    packet->stream_index    = context->videoStream->index;
    packet->dts             = AV_NOPTS_VALUE;
    if (IS_MSEC(timestamp)) {
        if (context->videoFirstTimestamp < 0) {
            context->videoFirstTimestamp = timestamp;
        }
        packet->pts = (timestamp - context->videoFirstTimestamp) * timeBaseDen / 1000;
    }
    else{
        packet->pts =  currentIndex * timeBaseDen / context->videoFrameRate;
    }
#ifdef WRITER_DEBUG
    printf("[XXfmMp4Writer] [video] %lld %d %d %d\r\n", packet->pts, currentIndex, context->videoFrameRate, timeBaseDen);
#endif
    
    // 写入
    if (av_interleaved_write_frame(context->context, packet) < 0) {
        av_packet_free(&packet);
        return -1;
    }
    av_packet_free(&packet);
    return 0;
}
int xxfmMp4Writer_audio(XXfmMp4WriterHandle handle, uint8_t *data, int length, int64_t timestamp){
    XXfmMp4WriterContext *context   = (XXfmMp4WriterContext*)handle;
    if (NULL == context || (NULL == data && length != 0) || NULL == context->audioStream) { //} || 0 != waitToLocked(context)) {
        return -1;
    }
    
    // 等待视频关键帧写入
    if (!context->isInited) {
        //unlock(context);
        return -1;
    }
    
    // 组成AVPacket
    int currentIndex    = context->audioFrameCount++;
    int timeBaseDen     = context->audioStream->time_base.den;
    int frameSize       = context->audioCodecContext->frame_size;

    AVPacket *packet        = av_packet_alloc();
    packet->data            = data;
    packet->size            = length;
    
    // 过滤流媒体数据头
    AVPacket *packetAfterBSF = av_packet_alloc();
    if(av_bsf_send_packet(context->audioBSFContext, packet)){
        av_packet_free(&packet);
        av_packet_free(&packetAfterBSF);
        return -1;
    }
    if(av_bsf_receive_packet(context->audioBSFContext, packetAfterBSF)){
        av_packet_free(&packet);
        av_packet_free(&packetAfterBSF);
        return -1;
    }
    
    packetAfterBSF->stream_index    = context->audioStream->index;
    packetAfterBSF->dts             = AV_NOPTS_VALUE;
    if (IS_MSEC(timestamp)) {
        if (context->audioFirstTimestamp < 0) {
            context->audioFirstTimestamp = timestamp;
        }
        packetAfterBSF->pts = (timestamp - context->audioFirstTimestamp) * timeBaseDen / 1000;
    }
    else{
        packetAfterBSF->pts = currentIndex * (frameSize * 1000 / context->audioSampleRate) * (timeBaseDen / 1000);
    }
#ifdef WRITER_DEBUG
    printf("[XXfmMp4Writer] [audio] %lld %d %d %d %d\r\n", packetAfterBSF->pts, currentIndex, frameSize, context->audioSampleRate, timeBaseDen);
#endif
    
    // 写入
    if (av_interleaved_write_frame(context->context, packetAfterBSF) < 0) {
        av_packet_free(&packet);
        av_packet_free(&packetAfterBSF);
        return -1;
    }
    av_packet_free(&packet);
    av_packet_free(&packetAfterBSF);
    return 0;
}
// 关闭并释放资源
void xxfmMp4Writer_close(XXfmMp4WriterHandle *handle, bool save){
    if(NULL == handle){
        return;
    }
    XXfmMp4WriterContext *context = *(XXfmMp4WriterContext**)handle;
    if (NULL == context){
        return;
    }
    
    if (save && context->isInited) {
        if(0!=av_write_trailer(context->context)){
#ifdef WRITER_DEBUG
            printf("[XXfmMp4Writer] failure to write trailer\r\n");
#endif
        }
        else{
#ifdef WRITER_DEBUG
            printf("[XXfmMp4Writer] succeed to write trailer\r\n");
#endif
        }
    }
    
//    if (context->videoBSFContext) {
//        av_bsf_free(&(context->videoBSFContext));
//    }
    if (context->videoCodecContext) {
        if (context->isInited) {
            avcodec_close(context->videoCodecContext);
        }
        avcodec_free_context(&(context->videoCodecContext));
    }
    
    if (context->audioBSFContext) {
        av_bsf_free(&(context->audioBSFContext));
    }
    if (context->audioCodecContext) {
        avcodec_close(context->audioCodecContext);
        avcodec_free_context(&(context->audioCodecContext));
    }
    avio_close(context->context->pb);
    
    if (save && context->isInited) {
        replaceStringInFile(context->context->filename, HEV1, HVC1, strlen(HEV1));
    }
    else{
        int ret = remove(context->context->filename);
#ifdef WRITER_DEBUG
        printf("[XXfmMp4Writer] remove file(%s), ret=%d\r\n", context->context->filename, ret);
#endif
    }
    avformat_free_context(context->context);
    free(context);
    *handle = NULL;
}

//
// 私有
//
static AVCodecContext* createCodecContext(enum AVCodecID codecID, void *param){
    AVCodec *codec = avcodec_find_decoder(codecID);
    if (NULL == codec) {
        return NULL;
    }
    
    AVCodecContext *codecContext = NULL;
    if (AVMEDIA_TYPE_AUDIO == codec->type) {
        XXfmMp4WriterAudioParam *audioParam = (XXfmMp4WriterAudioParam*)param;
        codecContext                        = avcodec_alloc_context3(codec);
        
        codecContext->codec_type    = AVMEDIA_TYPE_AUDIO;
        codecContext->codec_id      = codecID;
        codecContext->codec_tag     = 0;
        codecContext->sample_rate   = audioParam->sampleRate;
        
        codecContext->sample_fmt        = (enum AVSampleFormat)(audioParam->sampleFormat);
        codecContext->channels          = audioParam->channelCount;
        codecContext->channel_layout    = av_get_channel_layout(av_get_channel_name(audioParam->channelCount));
        codecContext->time_base         = av_make_q(1, audioParam->sampleRate);
        
        configAudioCodecContext(codecContext, audioParam);
    }
    else if(AVMEDIA_TYPE_VIDEO == codec->type){
        XXfmMp4WriterVideoParam *videoParam = (XXfmMp4WriterVideoParam*)param;
        codecContext                        = avcodec_alloc_context3(codec);
        
        codecContext->codec_type    = AVMEDIA_TYPE_VIDEO;
        codecContext->codec_id      = codecID;
        codecContext->codec_tag     = 0;
        
        codecContext->width         = videoParam->width;
        codecContext->height        = videoParam->height;
        codecContext->framerate     = av_make_q(videoParam->frameRate, 1);
        codecContext->time_base     = av_make_q(1, 90000);
    }
    else{
        return NULL;
    }
    
    if (codecContext->flags & AVFMT_GLOBALHEADER)
        codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    
    // 注意:由于视频需要在配置vps/sps/pps之后,才可以codecpar进行配置!!!
    if(codecContext->codec_type != AVMEDIA_TYPE_VIDEO && avcodec_open2(codecContext, codec, NULL)){
        avcodec_free_context(&codecContext);
        return NULL;
    }
    return codecContext;
}

static AVStream* addStream(AVFormatContext *formatContext, AVCodecContext *codecContext){
    AVStream *stream = avformat_new_stream(formatContext, NULL);
    if (NULL == stream) {
        return NULL;
    }
    
    // 注意:由于视频需要在配置vps/sps/pps之后,才可以codecpar进行配置!!!
    if (codecContext->codec_type != AVMEDIA_TYPE_VIDEO) {
        avcodec_parameters_from_context(stream->codecpar, codecContext);
    }
    return stream;
}

static enum AVCodecID videoTypeToAVCodecID(enum XXfmMp4WriterVideoType type){
    switch (type) {
        case XXavFrameType_H264: return AV_CODEC_ID_H264;
        case XXavFrameType_H265: return AV_CODEC_ID_HEVC;
        default: return AV_CODEC_ID_NONE;
    }
}
static int makeAvcc(AVCodecContext *context, XXavNalu *sps, XXavNalu *pps){
    if(NULL == context || NULL == sps || NULL == pps){
        return -1;
    }
    
    int extradataSize       = 5 + 3 + sps->length + 3 + pps->length;
    uint8_t *extradata      = av_mallocz(extradataSize);
    context->extradata_size = extradataSize;
    context->extradata      = extradata;
    
    // sps
    *extradata = 0x01;              extradata += 1;
    *extradata = sps->data[1];      extradata += 1;
    *extradata = sps->data[2];      extradata += 1;
    *extradata = sps->data[3];      extradata += 1;
    *extradata = 0xfc | 3;          extradata += 1;
    
    *extradata = 0xe0 | 1;                      extradata += 1;
    *extradata = sps->length >> 8;              extradata += 1;
    *extradata = sps->length;                   extradata += 1;
    memcpy(extradata, sps->data, sps->length);  extradata += sps->length;
    
    // pps
    *extradata = 1;                             extradata += 1;
    *extradata = pps->length >> 8;              extradata += 1;
    *extradata = pps->length;                   extradata += 1;
    memcpy(extradata, pps->data, pps->length);  extradata += pps->length;
    
    return 0;
}
static int makeHvcc(AVCodecContext *context, XXavNalu *sps, XXavNalu *pps, XXavNalu *vps){
    if(NULL == context || NULL == sps || NULL == pps || NULL == vps){
        return -1;
    }
    
    int extradataSize       = 23 + 5 + vps->length + 5 + sps->length+ 5 + pps->length;
    uint8_t *extradata      = av_mallocz(extradataSize);
    context->extradata_size = extradataSize;
    context->extradata      = extradata;
    
    /* hvcc */
    *extradata = 0x01;          extradata += 1;
    *extradata = 0x00;          extradata += 1;
    
    memset(extradata, 0, 4);    extradata += 4;
    memset(extradata, 0, 6);    extradata += 6;
    memset(extradata, 0, 9);    extradata += 9;
    
    *extradata = 0x03;          extradata += 1;
    *extradata = 0x03;          extradata += 1;
    
    // vps
    *extradata = 0x20;                          extradata += 1;
    *extradata = 0x00;                          extradata += 1;
    *extradata = 0x01;                          extradata += 1;
    *extradata = vps->length >> 8;              extradata += 1;
    *extradata = vps->length;                   extradata += 1;
    memcpy(extradata, vps->data, vps->length);  extradata += vps->length;
    
    // sps
    *extradata = 0x21;                          extradata += 1;
    *extradata = 0x00;                          extradata += 1;
    *extradata = 0x01;                          extradata += 1;
    *extradata = sps->length >> 8;              extradata += 1;
    *extradata = sps->length;                   extradata += 1;
    memcpy(extradata, sps->data, sps->length);  extradata += sps->length;
    
    // pps
    *extradata = 0x22;                          extradata += 1;
    *extradata = 0x00;                          extradata += 1;
    *extradata = 0x01;                          extradata += 1;
    *extradata = pps->length >> 8;              extradata += 1;
    *extradata = pps->length;                   extradata += 1;
    memcpy(extradata, pps->data, pps->length);  extradata += pps->length;
    
    return 0;
}
static int initVideoContext(XXfmMp4WriterContext *context, XXavNalu *vps, XXavNalu *sps, XXavNalu *pps){
    if (NULL == vps) {
        makeAvcc(context->videoCodecContext, sps, pps);
    }
    else{
        makeHvcc(context->videoCodecContext, sps, pps, vps);
    }
    
    if(avcodec_open2(context->videoCodecContext, avcodec_find_decoder(context->videoCodecContext->codec_id), NULL)){
        return -1;
    }
    if(avcodec_parameters_from_context(context->videoStream->codecpar, context->videoCodecContext) < 0){
        return -1;
    }
    if(avformat_write_header(context->context, NULL)){
        return -1;
    }
    
    return 0;
}

static enum AVCodecID audioTypeToAVCodecID(enum XXfmMp4WriterAudioType type){
    switch (type) {
        case XXavFrameType_AAC: return AV_CODEC_ID_AAC;
        default: return AV_CODEC_ID_NONE;
    }
}

static void makeAdts(AVCodecContext *context, int sampleRate, uint8_t channel){
    uint8_t objectType = 2; // AAC LC
    uint8_t sampleRateIndex;
    switch (sampleRate) {
        case 96000: sampleRateIndex = 0; break;
        case 88200: sampleRateIndex = 1; break;
        case 64000: sampleRateIndex = 2; break;
        case 48000: sampleRateIndex = 3; break;
        case 44100: sampleRateIndex = 4; break;
        case 32000: sampleRateIndex = 5; break;
        case 24000: sampleRateIndex = 6; break;
        case 22050: sampleRateIndex = 7; break;
        case 16000: sampleRateIndex = 8; break;
        case 12000: sampleRateIndex = 9; break;
        case 11025: sampleRateIndex = 10; break;
        case 8000:  sampleRateIndex = 11; break;
        case 7350:  sampleRateIndex = 12; break;
        default:    sampleRateIndex = -1; break;
    }
    
    context->extradata_size = 2;
    context->extradata      = av_mallocz(2);
    context->extradata[0]   = (objectType << 3) | (sampleRateIndex >> 1);
    context->extradata[1]   = ((sampleRateIndex&1)<<7) | (channel<<3);
}
static void configAudioCodecContext(AVCodecContext *codecContext, XXfmMp4WriterAudioParam *param){
    switch (codecContext->codec_id) {
        case AV_CODEC_ID_AAC:
            codecContext->frame_size        = 1024; // aac固定长度
            codecContext->profile           = 0x02;
            makeAdts(codecContext, param->sampleRate, param->channelCount);
            break;
        default:
            break;
    }
}

static int splitNalu(XXavNalu naluList[], int naluListMax, uint8_t *data, int length, bool isH264){
    uint8_t naluCount           = 0;
    int index                   = 0;
    int previousNaluOffset      = -1;
    int currentNaluOffset       = -1;
    
    while (index < length - 4) {
        if(0 == memcmp(START_CODE, data+index, 4)){
            index += 4;
            if (naluCount >= naluListMax) {
                break;
            }
            
            currentNaluOffset           = index;                                    // 记录nale数据起始偏移
            naluList[naluCount].data    = data + currentNaluOffset;                 // 记录nale数据起始地址,注意:此处直接使用源数据的地址
            naluList[naluCount].length  = 0;                                        // 由于本nalu的长度需要获取下一个nalu的startCode位置才可以计算,所以先置为0
            naluList[naluCount].type    = toXXavNaluType(data + index, isH264);     // 将nale类型转换到统一的类型定义
            if (naluCount > 0) {
                naluList[naluCount - 1].length = index - 4 - previousNaluOffset ;   // 计算上一个nale的数据长度
            }
            
            previousNaluOffset  = currentNaluOffset;
            naluCount           += 1;
        }
        else{
            index++;
        }
    }
    
    if (naluCount > 0) {
        naluList[naluCount - 1].length = length - previousNaluOffset;    // 记录上一个nalu的长度
    }
    return naluCount;
}
static XXavNaluType toXXavNaluType(uint8_t *data, bool isH264){
    if(isH264){
        uint8_t type = *data & 0x1f;
        switch (type) {
            case H264_SPS_CODEC:    return XXavNaluType_SPS;
            case H264_PPS_CODEC:    return XXavNaluType_PPS;
            case H264_IDR_CODEC:    return XXavNaluType_IFrame;
            case H264_P_CODEC:      return XXavNaluType_PFrame;
            default:                return XXavNaluType_Unknown;
        }
    }
    else{
        uint8_t type = (*data >> 1) & 0x3f;
        switch (type) {
            case H265_VPS_CODEC:    return XXavNaluType_VPS;
            case H265_SPS_CODEC:    return XXavNaluType_SPS;
            case H265_PPS_CODEC:    return XXavNaluType_PPS;
            case H265_IDR_CODEC:    return XXavNaluType_IFrame;
            case H265_P_CODEC:      return XXavNaluType_PFrame;
            default:                return XXavNaluType_Unknown;
        }
    }
}
static int replaceStringInFile(const char *filePath, const char *src, const char *dest, int length){
    FILE *file = fopen(filePath, "r+");
    if (NULL == file) {
        return -1;
    }
    
    fseek(file, 0, SEEK_SET);
    int matchCount  = 0;
    bool isFound    = false;
    
    while (!feof(file)){
        char ch = fgetc(file);
        if (src[matchCount] == ch) {
            matchCount++;
            if (matchCount >= length) {
                isFound = true;
                fseek(file, -length, SEEK_CUR);
                break;
            }
        }
        else{
            matchCount = 0;
        }
    }
    
    if (!isFound) {
#ifdef WRITER_DEBUG
        printf("[XXfmMp4Writer] could not find 'hev1'\r\n");
#endif
        fclose(file);
        return -1;
    }
    
    if(length != fwrite(dest, 1, length, file)){
#ifdef WRITER_DEBUG
        printf("[XXfmMp4Writer] could not replace hev1 with hvc1 by fwrite (%ld).\r\n", ftell(file));
#endif
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return 0;
}
