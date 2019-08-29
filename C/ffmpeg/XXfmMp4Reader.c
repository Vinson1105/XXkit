#include "XXfmMp4Reader.h"
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

typedef struct XXfmMp4ReaderContext{
    AVFormatContext *formatContext;
    AVCodecContext *videoCodecContext;
    AVCodecContext *audioCodecContext;
    AVStream *videoStream;
    AVStream *audioStream;
    int videoStreamIndex;
    int audioStreamIndex;
    bool needDecode;
    
    AVPacket *packet;
    AVFrame *frame;
}XXfmMp4ReaderContext;

static void toYuv420p(int8_t *buffer, AVFrame *frame);

XXfmMp4ReaderHandle xxfmMp4Reader_create(const char *path, bool needDecode){
    AVFormatContext *formatContext      = NULL;
    AVCodecContext *videoCodecContext   = NULL;
    AVCodecContext *audioCodecContext   = NULL;
    AVStream *videoStream               = NULL;
    AVStream *audioStream               = NULL;
    int videoStreamIndex                = 0;
    int audioStreamIndex                = 0;
    
    AVCodec *videoCodec = NULL;
    AVCodec *audioCodec = NULL;
    int streamCount     = 0;
    int streamIndex     = 0;
    
    // 打开输入文件
    av_register_all();
    avcodec_register_all();
    if(avformat_open_input(&formatContext, path, NULL, NULL)){
        printf("[XXfmMp4Reader] can not open file.(%s)\n", path);
        goto FailureToCreate;
    }
    if(avformat_find_stream_info(formatContext, NULL) < 0){
        printf("[XXfmMp4Reader] can not find stream info.\n");
        goto FailureToCreate;
    }
    
    // 查找音视频轨
    streamCount = formatContext->nb_streams;
//    if (2 != streamCount) {
//        printf("[XXfmMp4Reader] count of stream is not 2.\n");
//        goto FailureToCreate;
//    }
    for (streamIndex = 0; streamIndex < streamCount; streamIndex++) {
        AVStream *stream = formatContext->streams[streamIndex];
        if (AVMEDIA_TYPE_VIDEO == stream->codecpar->codec_type) {
            videoStream = stream;
            videoStreamIndex = streamIndex;
        }
        else if(AVMEDIA_TYPE_AUDIO == stream->codecpar->codec_type){
            audioStream = stream;
            audioStreamIndex = streamIndex;
        }
        else{
            
        }
    }
    if (NULL == videoStream || NULL == audioStream) {
        printf("[XXfmMp4Reader] can not find audio or video stream.\n");
        goto FailureToCreate;
    }
    
    // 打开音视频编码器
    if (needDecode) {
        videoCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
        videoCodecContext = avcodec_alloc_context3(videoCodec);
        avcodec_parameters_to_context(videoCodecContext, videoStream->codecpar);
        if (avcodec_open2(videoCodecContext, videoCodec, NULL)) {
            printf("[XXfmMp4Reader] can not open video codec.\n");
            goto FailureToCreate;
        }
        
        audioCodec = avcodec_find_decoder(audioStream->codecpar->codec_id);
        audioCodecContext = avcodec_alloc_context3(audioCodec);
        avcodec_parameters_to_context(audioCodecContext, audioStream->codecpar);
        if (avcodec_open2(audioCodecContext, audioCodec, NULL)) {
            printf("[XXfmMp4Reader] can not open audio codec.\n");
            goto FailureToCreate;
        }
    }
    
    // 分配handle
    XXfmMp4ReaderContext *context = (XXfmMp4ReaderContext*)malloc(sizeof(XXfmMp4ReaderContext));
    context->formatContext = formatContext;
    context->videoCodecContext = videoCodecContext;
    context->audioCodecContext = audioCodecContext;
    context->videoStream = videoStream;
    context->audioStream = audioStream;
    context->needDecode = needDecode;
    context->videoStreamIndex = videoStreamIndex;
    context->audioStreamIndex = audioStreamIndex;
    
    context->packet = av_packet_alloc();
    context->frame = av_frame_alloc();
    return context;
    
FailureToCreate:
    if (NULL != audioCodecContext) {
        avcodec_free_context(&audioCodecContext);
    }
    if (NULL != videoCodecContext) {
        avcodec_free_context(&videoCodecContext);
    }
    if (NULL != formatContext) {
        avformat_close_input(&formatContext);
    }
    return NULL;
}
int xxfmMp4Reader_next(XXfmMp4ReaderHandle handle, XXavFrame *frame){
    // 转换到context
    XXfmMp4ReaderContext *context = (XXfmMp4ReaderContext*)handle;
    if (NULL == handle) {
        return -1;
    }
    
    // 读取帧数据
    AVFormatContext *formatContext = context->formatContext;
    if(av_read_frame(formatContext, context->packet)){
        return XXFM_MP4_READER_IS_END_OF_FILE;
    }

    // 找出帧数据对应的解码器
    AVCodecContext *codecContext = NULL;
    bool isVideoFrame = false;
    bool isKeyFrame = false;
    if (context->packet->stream_index == context->videoStream->index) {
        codecContext = context->videoCodecContext;
        isVideoFrame = true;
        isKeyFrame = context->packet->flags & AV_PKT_FLAG_KEY;
    }
    else if(context->packet->stream_index == context->audioStream->index){
        codecContext = context->audioCodecContext;
        isVideoFrame = false;
    }
    else{
        return -1;
    }
    
    // 解码
    int ret = avcodec_send_packet(codecContext, context->packet);
    if(ret){
        return -1;
    }
    ret = avcodec_receive_frame(codecContext, context->frame);
    if(ret == AVERROR(EAGAIN)){
        return XXFM_MP4_READER_NEXT;
    }
    else {
        if (0 != ret) {
            return -1;
        }
    }
    
    // 整合数据
    int timebase    = isVideoFrame ? context->videoStream->time_base.den : context->audioStream->time_base.den;
    frame->dts      = context->frame->pkt_dts * 1000 / timebase;
    frame->pts      = context->frame->pkt_pts * 1000 / timebase;
    frame->isVideo  = isVideoFrame;
    frame->isKey    = isKeyFrame;
    
    if (NULL != frame->data) {
        free(frame->data);
    }
    if (isVideoFrame) {
        frame->length = context->frame->width * context->frame->height * 3 / 2;
        frame->data = malloc((int)(frame->length));
        toYuv420p(frame->data, context->frame);
    }
    else{
        frame->length = context->frame->linesize[0];
        frame->data = malloc(context->frame->linesize[0]);
        memcpy(frame->data, context->frame->data[0], context->frame->linesize[0]);
    }
    
    return 0;
}
int xxfmMp4Reader_seek(XXfmMp4ReaderHandle handle, uint64_t msec){
    XXfmMp4ReaderContext *context = (XXfmMp4ReaderContext*)handle;
    if (NULL == handle) {
        return -1;
    }
    
    int64_t timestamp = msec * context->videoStream->time_base.den / 1000;
    av_seek_frame(context->formatContext, context->videoStreamIndex, timestamp, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(context->videoCodecContext);
    return 0;
}
void xxfmMp4Reader_free(XXfmMp4ReaderHandle *handle){
    XXfmMp4ReaderContext **context = (XXfmMp4ReaderContext**)handle;
    if (NULL == context || NULL == *context) {
        return;
    }
    
    if (NULL != (*context)->audioCodecContext) {
        avcodec_free_context(&((*context)->audioCodecContext));
    }
    if (NULL != (*context)->videoCodecContext) {
        avcodec_free_context(&((*context)->videoCodecContext));
    }
    
    if (NULL != (*context)->formatContext) {
        avformat_close_input(&((*context)->formatContext));
    }
    free(*context);
    *context = NULL;
}

int xxfmMp4Reader_resolution(XXfmMp4ReaderHandle handle, int *width, int *height){
    XXfmMp4ReaderContext *context = (XXfmMp4ReaderContext*)handle;
    if (NULL == context || NULL == context->videoStream) {
        return -1;
    }

    *width = context->videoStream->codecpar->width;
    *height = context->videoStream->codecpar->height;
    return 0;
}

static void toYuv420p(int8_t *buffer, AVFrame *frame){
    int offset          = 0, i;
    int halfOfWidth     = frame->width >> 1;
    int halfOfHeight    = frame->height >> 1;
    
    for (i = 0; i < frame->height; i++){
        memcpy(buffer + offset, frame->data[0] + i * frame->linesize[0], frame->width);
        offset += frame->width;
    }
    for (i = 0; i < halfOfHeight; i++){
        memcpy(buffer + offset, frame->data[1] + i * frame->linesize[1], halfOfWidth);
        offset += halfOfWidth;
    }
    for (i = 0; i < halfOfHeight; i++){
        memcpy(buffer + offset, frame->data[2] + i * frame->linesize[2], halfOfWidth);
        offset += halfOfWidth;
    }
}
