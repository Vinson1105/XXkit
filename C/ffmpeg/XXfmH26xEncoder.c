//
//  XXfmH26xEncoder.c
//  NVSDK
//
//  Created by VINSON on 2020/8/10.
//  Copyright © 2020 macrovideo. All rights reserved.
//

#include "XXfmH26xEncoder.h"
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>

#define kBufferLengthMax 2048*2048*3

typedef struct XXfmH26xEncoderContext{
    AVCodec         *codec;
    AVCodecContext  *codecContext;
    AVFrame         *frame;
    AVPacket        *packet;
}XXfmH26xEncoderContext;

static XXfmH26xEncoderHandle allocHandle(enum AVCodecID codecId, int width, int height, enum AVPixelFormat pix_fmt);
static int encode(XXfmH26xEncoderContext *context);
static int toFrame(const uint8_t *src, uint32_t width, uint32_t height, AVFrame *frame);

XXfmH26xEncoderHandle xxfmH264Encoder_create(int width, int height, XXfmH26xEncoderFormatType format){
    enum AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;
    if(format==XXfmH26xEncoderFormatTypeRgb24) pix_fmt = AV_PIX_FMT_RGB24;
    if(format==XXfmH26xEncoderFormatTypeYuv420)pix_fmt = AV_PIX_FMT_YUV420P;
    if(pix_fmt==AV_PIX_FMT_NONE){
        return NULL;
    }
    return allocHandle(AV_CODEC_ID_H264,width,height,pix_fmt);
}
XXfmH26xEncoderHandle xxfmH265Encoder_create(int width, int height,XXfmH26xEncoderFormatType format){
    enum AVPixelFormat pix_fmt = AV_PIX_FMT_NONE;
    if(format==XXfmH26xEncoderFormatTypeRgb24) pix_fmt = AV_PIX_FMT_RGB24;
    if(format==XXfmH26xEncoderFormatTypeYuv420)pix_fmt = AV_PIX_FMT_YUV420P;
    if(pix_fmt==AV_PIX_FMT_NONE){
        return NULL;
    }
    return allocHandle(AV_CODEC_ID_H265,width,height,pix_fmt);
}

int xxfmH26xEncoder_encode(XXfmH26xEncoderHandle handle, const uint8_t *srcData, int srcLength, const uint8_t *destData, int *destLength, bool *keyFrame){
    XXfmH26xEncoderContext *context = (XXfmH26xEncoderContext*)(handle);
    if (NULL == context || NULL == context->frame || NULL == context->packet){
        return -1;
    }
    
    if(context->codecContext->pix_fmt == AV_PIX_FMT_YUV420P){
        toFrame(srcData, context->codecContext->width, context->codecContext->height, context->frame);
    }
    else if(context->codecContext->pix_fmt == AV_PIX_FMT_RGB24){
        return -1;
    }
    else{
        return -1;
    }
    
    int ret = encode(context);
    if(0 != ret){
        printf("[XXfmH26xEncoder] [encode] 编码失败，%d(%s)\n", ret, av_err2str(ret));
        return ret;
    }
    
    memcpy((void *)destData, context->packet->data, context->packet->size);
    *destLength = context->packet->size;
    *keyFrame = context->packet->flags & AV_PKT_FLAG_KEY;
    printf("[XXfmH26xEncoder] [encode] 编码成功，%s\n", *keyFrame?"关键帧":"非关键帧");
    return 0;
}
void xxfmH26xEncoder_free(XXfmH26xEncoderHandle *handle){
    if(NULL == handle || NULL == *(XXfmH26xEncoderContext**)handle){
        return;
    }
    
    XXfmH26xEncoderContext *context = *(XXfmH26xEncoderContext**)(handle);
    if (NULL != context->packet){
        av_packet_free(&(context->packet));
    }
    if (NULL != context->frame){
        av_frame_free(&(context->frame));
    }
    if (NULL != context->codecContext){
        avcodec_close(context->codecContext);
        av_free(context->codecContext);
    }
    free(context);
}

static XXfmH26xEncoderHandle allocHandle(enum AVCodecID codecId, int width, int height, enum AVPixelFormat pix_fmt){
    // [1] ffmpeg注册,返回值初始化
    printf("[XXfmH26xEncoder] [allocHandle] 开始\n");
    av_register_all();
    avcodec_register_all();

    // [2]
    XXfmH26xEncoderContext* context = (XXfmH26xEncoderContext*)malloc(sizeof(XXfmH26xEncoderContext));
    memset(context, 0, sizeof(XXfmH26xEncoderContext));

    // [3] 获取h26x编码器指针
    context->codec = avcodec_find_encoder(codecId);
    if (NULL == context->codec){
        printf("[XXfmH26xEncoder] [allocHandle] 无法找到编码器（codecID=%d codec=%s）\n", codecId, avcodec_get_name(codecId));
        goto FailureToAlloc;
    }

    // [4] 构建h26x编码器容器，并设置编码参数
    context->codecContext = avcodec_alloc_context3(context->codec);
    if (NULL == context->codecContext){
        goto FailureToAlloc;
    }
    context->codecContext->time_base = av_make_q(1,12);
    context->codecContext->width = width;
    context->codecContext->height = height;
    context->codecContext->gop_size = 12;
    context->codecContext->pix_fmt = pix_fmt;
//    context->codecContext->bit_rate = width*height*2*12;
//    context->codecContext->rc_max_rate = width*height*3*12;
//    context->codecContext->rc_min_rate = width*height*12;
    
    AVDictionary *opts = NULL;
    if(codecId == AV_CODEC_ID_H264){
        av_dict_set(&opts,  "preset" ,  "slow" , 0);
        av_dict_set(&opts,  "tune" ,  "zerolatency" , 0);
        av_dict_set(&opts, "profile", "baseline", 0);
    }

    // [5] 打开解码器
    if (avcodec_open2(context->codecContext, context->codec, &opts) < 0){
        printf("[XXfmH26xEncoder] [allocHandle] 无法打开编码器\n");
        goto FailureToAlloc;
    }

    // [6] 分配其他空间
    context->frame = av_frame_alloc();
    if (NULL == context->frame){
        printf("[XXfmH26xEncoder] [allocHandle] frame分配失败\n");
        goto FailureToAlloc;
    }
    context->frame->format = pix_fmt;
    context->frame->width  = width;
    context->frame->height = height;
    if(av_frame_get_buffer(context->frame, 8) != 0){
        printf("[XXfmH26xEncoder] [allocHandle] frame获取buffer失败\n");
        goto FailureToAlloc;
    }
    
    context->packet = av_packet_alloc();
    av_init_packet(context->packet);
    
    // [7] 其他初始化
    printf("[XXfmH26xEncoder] [allocHandle] 创建成功\n");
    if(NULL != opts){
        av_dict_free(&opts);
    }
    return context;

FailureToAlloc:
    xxfmH26xEncoder_free((XXfmH26xEncoderHandle*)&context);
    return NULL;
}
static int encode(XXfmH26xEncoderContext *context){
    int res = avcodec_send_frame(context->codecContext, context->frame);
    if(0 != res){
        printf("[XXfmH26xEncoder] [encode] [avcodec_send_frame] 编码失败，%d(%s)\n", res, av_err2str(res));
        return res;
    }

    res = avcodec_receive_packet(context->codecContext, context->packet);
    if(0 != res){
        printf("[XXfmH26xEncoder] [encode] [avcodec_receive_packet] 编码失败，%d(%s)\n", res, av_err2str(res));
        return res;
    }
    return 0;
}
static int toFrame(const uint8_t *src, uint32_t width, uint32_t height, AVFrame *frame){
    int ret = av_frame_make_writable(frame);
    if (ret < 0) {
        printf("[XXfmH26xEncoder] [toFrame] could not make sure the frame data is writable.\n");
        return ret;
    }

    int offset = 0, i;
    uint32_t halfOfWidth = width >> 1;
    uint32_t halfOfHeight = height >> 1;
    
    for (i = 0; i < height; i++)
    {
        memcpy(frame->data[0] + i * frame->linesize[0], src + offset, width);
        offset += width;
    }
    for (i = 0; i < halfOfHeight; i++)
    {
        memcpy(frame->data[1] + i * frame->linesize[1], src + offset,  halfOfWidth);
        offset += halfOfWidth;
    }
    for (i = 0; i < halfOfHeight; i++)
    {
        memcpy(frame->data[2] + i * frame->linesize[2], src + offset, halfOfWidth);
        offset += halfOfWidth;
    }
    frame->pts++;
    return 0;
}
