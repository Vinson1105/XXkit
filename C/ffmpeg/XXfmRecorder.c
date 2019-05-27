#include "XXfmRecorder.h"
#include "XXfmUtility.h"
#include <stdbool.h>
#include <pthread.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/audio_fifo.h>
//#include <libavutil/avutil.h>

#define DECLARE_CONTEXT(handle) XXfmRecorderContext *context = (XXfmRecorderContext*)handle;

typedef struct XXfmRecorderContext{
    XXfmRecorderAudioParam audioParam;
    XXfmRecorderVideoParam videoParam;
    void *opaque;
    XXfmRecorderGotVideoFunc gotVideoFunc;
    XXfmRecorderGotAudioFunc gotAudioFunc;
    XXfmRecorderResultFunc resultFunc;

    AVFormatContext *videoFormatContext;
    AVStream *videoStream;
    AVStream *audioStream;
    AVCodecContext *videoDecodeContext;
    AVCodecContext *videoEncodeContext;
    struct SwsContext *videoSwsContext;

    bool isRunning;
    bool isInterruptionRequested;
}XXfmRecorderContext;

XXfmRecorderHandle xxfmRecorder_create(void *opaque,
                                       XXfmRecorderGotVideoFunc gotVideoFunc,
                                       XXfmRecorderGotAudioFunc gotAudioFunc,
                                       XXfmRecorderResultFunc resultFunc,
                                       XXfmRecorderVideoParam *videoParam,
                                       XXfmRecorderAudioParam *audioParam){
    av_register_all();
    avdevice_register_all();

    XXfmRecorderContext *context = NULL;
    AVStream *videoStream = NULL;
    AVStream *audioStream = NULL;

    AVFormatContext *videoFormatContext = NULL;
    AVCodecContext *videoDecodeContext  = NULL;
    AVCodecContext *videoEncodeContext  = NULL;
    struct SwsContext *videoSwsContext  = NULL;

    char bytes[12]                      = {0};
    AVInputFormat *videoInputFormat     = NULL;
    AVDictionary *options               = NULL;


    // 检查输入参数
    if(NULL == videoParam || NULL == audioParam){
        goto XXfmRecorderCreateEnd;
    }

    // 搜索输入设备（暂时是录屏）
    videoInputFormat = av_find_input_format("gdigrab");
    if(NULL == videoInputFormat){
        goto XXfmRecorderCreateEnd;
    }

    // 打开输入设备
    memset(bytes, 0, 12);
    sprintf(bytes, "%d", videoParam->framerate);
    av_dict_set(&options, "framerate",  bytes, NULL);
    memset(bytes, 0, 12);
    sprintf(bytes, "%dx%d", videoParam->desktopWidth, videoParam->desktopHeight);
    av_dict_set(&options,"video_size", bytes, NULL);
    if(0 != avformat_open_input(&videoFormatContext, "desktop", videoInputFormat, &options)){
        goto XXfmRecorderCreateEnd;
    }

    // 寻找视频轨
    videoStream = xxfmUtility_findVideoStream(videoFormatContext);
    if(NULL == videoStream){
        goto XXfmRecorderCreateEnd;
    }

    // 打开视频解码器
    if(xxfmUtility_openDecodeWithStream(&videoDecodeContext, videoStream) < 0){
         goto XXfmRecorderCreateEnd;
    }

    // 打开视频编码器
    if(xxfmUtility_openEncodeWithParam(&videoEncodeContext,
                                       AV_CODEC_ID_H264,
                                       AV_PIX_FMT_YUV420P,
                                       videoParam->width,
                                       videoParam->height,
                                       videoParam->framerate,
                                       400000,
                                       videoParam->framerate) < 0){
        goto XXfmRecorderCreateEnd;
    }

    // 打开视频重采样器
    videoSwsContext = sws_getContext(videoParam->desktopWidth, videoParam->desktopHeight, videoParam->desktopPixelFormat,
                                     videoEncodeContext->width, videoEncodeContext->height, videoEncodeContext->pix_fmt,
                                     SWS_BICUBIC, NULL, NULL, NULL);
    if(NULL == videoSwsContext){
        goto XXfmRecorderCreateEnd;
    }

    // 保存参数到handle
    context = (XXfmRecorderContext*)malloc(sizeof(XXfmRecorderContext));
    memset(&(context->audioParam), audioParam, sizeof(XXfmRecorderAudioParam));
    memset(&(context->videoParam), videoParam, sizeof(XXfmRecorderVideoParam));
    context->opaque         = opaque;
    context->gotVideoFunc   = gotVideoFunc;
    context->gotAudioFunc   = gotAudioFunc;
    context->resultFunc     = resultFunc;

    context->videoFormatContext = videoFormatContext;
    context->videoStream = videoStream;
    context->audioStream = audioStream;
    context->videoDecodeContext = videoDecodeContext;
    context->videoEncodeContext = videoEncodeContext;
    context->videoSwsContext    = videoSwsContext;

    return context;

XXfmRecorderCreateEnd:
    if(NULL != videoFormatContext) { avformat_close_input(videoFormatContext); avformat_free_context(videoFormatContext); }
    if(NULL != videoDecodeContext) { avcodec_close(videoDecodeContext); avcodec_free_context(&videoDecodeContext); }
    if(NULL != videoEncodeContext) { avcodec_close(videoEncodeContext); avcodec_free_context(&videoEncodeContext); }
    if(NULL != videoSwsContext) { sws_freeContext(videoSwsContext); }
    return NULL;
}
int xxfmRecorder_start(XXfmRecorderHandle handle){
    DECLARE_CONTEXT(handle)
    if(NULL == context){
        return -1;
    }

    int ret = 0;
    XXfmRecorderResult result   = XXfmRecorderResult_Finished;
    bool isFinished             = false;
    context->isRunning          = true;

    AVFormatContext *videoFormatContext = context->videoFormatContext;
    AVStream *videoStream = context->videoStream;

    AVCodecContext *videoDecodeContext  = context->videoDecodeContext;
    AVCodecContext *videoEncodeContext  = context->videoEncodeContext;
    struct SwsContext *videoSwsContext  = context->videoSwsContext;

    // 分配存储空间
    AVPacket *srcPacket = av_packet_alloc();
    AVPacket *destPacket = av_packet_alloc();

    AVFrame *frame = av_frame_alloc();
    AVFrame *yuvFrame = av_frame_alloc();
    uint8_t *yuvBuffer = (uint8_t*)av_malloc(avpicture_get_size(videoEncodeContext->pix_fmt, videoEncodeContext->width, videoEncodeContext->height));
    avpicture_fill((AVPicture *)yuvFrame, yuvBuffer, videoEncodeContext->pix_fmt, videoEncodeContext->width, videoEncodeContext->height);
    //av_image_fill_arrays();

    // 循环获取视频数据
    while (!context->isInterruptionRequested && !isFinished) {
        // 获取原始帧
        ret = av_read_frame(videoFormatContext, srcPacket);
        if(0 != ret){
            result = XXfmRecorderResult_FailureToReadFrame;
            goto XXfmRecorderStartEnd;
        }

        // 解码
        if(avcodec_send_packet(videoDecodeContext, srcPacket) < 0){
            result = XXfmRecorderResult_FailureToSendPacket;
            goto XXfmRecorderStartEnd;
        }

        ret = 0;
        while (ret >= 0 && !isFinished) {
            ret = avcodec_receive_frame(videoDecodeContext, frame);
            if (ret == AVERROR(EAGAIN)){
                break;
            }
            else if (ret == AVERROR_EOF){
                result = XXfmRecorderResult_Finished;
                isFinished = true;
            }
            else if (ret < 0) {
                result = XXfmRecorderResult_FailureToReceiveFrame;
                goto XXfmRecorderStartEnd;
            }
            else{

            }

            // 进行视频重采样
            ret = sws_scale(videoSwsContext, frame->data, frame->linesize, 0, videoDecodeContext->height, yuvFrame->data, yuvFrame->linesize);
            if(ret <= 0){
                result = XXfmRecorderResult_FailureToScale;
                goto XXfmRecorderStartEnd;
            }

            // 时间基转换以及其他参数赋值
            xxfmUtility_transformTimeBaseFromAnotherFrame(frame, videoStream->time_base, yuvFrame, videoEncodeContext->time_base);
            yuvFrame->width     = frame->width;
            yuvFrame->height    = frame->height;
            yuvFrame->format    = frame->format;

            // 进行视频编码
            ret = avcodec_send_frame(videoEncodeContext, yuvFrame);
            if(0 != ret){
                result = XXfmRecorderResult_FailureToSendFrame;
                goto XXfmRecorderStartEnd;
            }
            ret = avcodec_receive_packet(videoEncodeContext, destPacket);
            if(0 != ret){
                result = XXfmRecorderResult_FailureToReceivePacket;
                goto XXfmRecorderStartEnd;
            }

            // 回调通知
            context->gotVideoFunc(context->opaque,
                                  destPacket->data,
                                  destPacket->size,
                                  destPacket->pts * 1000 / videoEncodeContext->time_base.den,
                                  videoEncodeContext->width,
                                  videoEncodeContext->height,
                                  destPacket->flags & AV_PKT_FLAG_KEY);

            av_packet_unref(srcPacket);
            av_packet_unref(destPacket);
        }
    }

XXfmRecorderStartEnd:
    av_frame_free(&frame);
    av_frame_free(&yuvFrame);
    av_packet_free(&srcPacket);
    av_packet_free(&destPacket);

    context->isInterruptionRequested = false;
    context->isRunning = false;

    context->resultFunc(context->opaque, result);
    return -1;
}
void xxfmRecorder_stop(XXfmRecorderHandle handle){
    DECLARE_CONTEXT(handle)
    if(NULL == context){
        return -1;
    }

    if(context->isRunning){
        context->isInterruptionRequested = true;
    }
}
void xxfmRecorder_free(XXfmRecorderHandle *handlePtr){
    if(NULL == handlePtr){
        return;
    }

    DECLARE_CONTEXT(*handlePtr);
    if(NULL == context){
        return;
    }

    if(NULL != context->videoFormatContext) {
        avformat_close_input(context->videoFormatContext);
        avformat_free_context(context->videoFormatContext);
    }

    if(NULL != context->videoDecodeContext) {
        avcodec_close(context->videoDecodeContext);
        avcodec_free_context(&(context->videoDecodeContext));
    }

    if(NULL != context->videoEncodeContext) {
        avcodec_close(context->videoEncodeContext);
        avcodec_free_context(&(context->videoEncodeContext));
    }

    if(NULL != context->videoSwsContext)    {
        sws_freeContext(context->videoSwsContext);
    }

    free(context);
    *handlePtr = NULL;
}

static void run(void *param){
    DECLARE_CONTEXT(param)
}