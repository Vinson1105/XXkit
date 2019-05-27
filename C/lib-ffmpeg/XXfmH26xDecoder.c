#include "./XXfmH26xDecoder.h"
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

typedef struct XXfmH26xDecoderContext{
	AVCodec			*codec;		    // h26x编解码器对象
	AVCodecContext	*codecContext;	// h26x编解码器容器
	AVFrame			*frame;
	AVPacket		*packet;
}XXfmH26xDecoderContext;

static XXfmH26xDecoderHandle allocHandle(enum AVCodecID codecId);
static int decode(XXfmH26xDecoderContext *context);
//static void toYuv420p(char *dest, AVFrame *frame);
//static void toYuv422p(char *dest, AVFrame *frame);

XXfmH26xDecoderOutput* XXfmH26xDecoderOutput_alloc(void){
    XXfmH26xDecoderOutput *output = (XXfmH26xDecoderOutput*)malloc(sizeof(XXfmH26xDecoderOutput));
    output->width = 0;
    output->height = 0;
    for (int i = 0; i < 3; i++) {
        output->data[i] = NULL;
        output->length[i] = 0;
    }
    return output;
}
void XXfmH26xDecoderOutput_free(XXfmH26xDecoderOutput **output){
    if (NULL == output || NULL == *output) {
        return;
    }
    
    for (int i = 0; i < 3; i++) {
        if (NULL != (*output)->data[i]) {
            free((*output)->data[i]);
        }
    }
    free(*output);
    *output = NULL;
}

XXfmH26xDecoderHandle xxfmH264Decoder_create(){
    return allocHandle(AV_CODEC_ID_H264);
}
XXfmH26xDecoderHandle xxfmH265Decoder_create(){
    return allocHandle(AV_CODEC_ID_H265);
}
int xxfmH26xDecoder_decode(XXfmH26xDecoderHandle handle, const uint8_t *data, int length, XXfmH26xDecoderOutput *output){
	// [1]
	XXfmH26xDecoderContext *context = (XXfmH26xDecoderContext*)(handle);
	if (NULL == context){
		return -1;
	}
	context->packet->data = (uint8_t*)data;
	context->packet->size = length;

	int ret = decode(context);
	if (0 != ret){
		return -1;
	}

	// [2] 输出数据整理
	output->data[0]	= context->frame->data[0];
	output->data[1]	= context->frame->data[1];
	output->data[2]	= context->frame->data[2];

	output->length[0]	= context->frame->linesize[0];
	output->length[1]	= context->frame->linesize[1];
	output->length[2]	= context->frame->linesize[2];

	output->width		= context->frame->width;
	output->height		= context->frame->height;
	return 0;
}
void xxfmH26xDecoder_free(XXfmH26xDecoderHandle *handle){
    if(NULL == handle || NULL == *handle){
        return;
    }
    
	XXfmH26xDecoderContext *context = (XXfmH26xDecoderContext*)(handle);
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


//
//
//	以下是工具类函数
//
//
static XXfmH26xDecoderHandle allocHandle(enum AVCodecID codecId){
    // [1] ffmpeg解码器注册,返回值初始化
    av_register_all();
    avcodec_register_all();

    // [2] 
    XXfmH26xDecoderContext* context = (XXfmH26xDecoderContext*)malloc(sizeof(XXfmH26xDecoderContext));
    memset(context, 0, sizeof(XXfmH26xDecoderContext));

    // [3] 获取h26x解码器指针
    context->codec = avcodec_find_decoder(codecId);
    if (NULL == context->codec){
        goto FailureToAlloc;
    }

    // [4] 构建h26x解码器容器
    context->codecContext = avcodec_alloc_context3(context->codec);
    if (NULL == context->codecContext){
        goto FailureToAlloc;
    }

    // [5] 打开解码器
    if (avcodec_open2(context->codecContext, context->codec, NULL) < 0){
        goto FailureToAlloc;
    }

    // [6] 分配其他空间
    context->frame = av_frame_alloc();
    if (NULL == context->frame){
        goto FailureToAlloc;
    }
    context->packet = av_packet_alloc();
    av_init_packet(context->packet);
    return context;

FailureToAlloc:
    xxfmH26xDecoder_free((XXfmH26xDecoderHandle*)&context);
    return NULL;
}
static int decode(XXfmH26xDecoderContext *context){
	// [2] 解码一帧数据	
	int res = avcodec_send_packet(context->codecContext, context->packet);
	if (res != 0){
        return -1;
	}

	// [3] 获取已解码数据
	res = avcodec_receive_frame(context->codecContext, context->frame);
	if (res != 0){
        return -1;
	}
	return 0;
}
//void toYuv420p(char *pDest, AVFrame *pFrame)
//{
//
//    int offset = 0, i;
//    XfmU32 halfOfWidth = pFrame->width >> 1;
//    XfmU32 halfOfHeight = pFrame->height >> 1;
//    for (i = 0; i < pFrame->height; i++)
//    {
//        memcpy(pDest + offset, pFrame->data[0] + i * pFrame->linesize[0], pFrame->width);
//        offset += pFrame->width;
//    }
//    for (i = 0; i < halfOfHeight; i++)
//    {
//        memcpy(pDest + offset, pFrame->data[1] + i * pFrame->linesize[1], halfOfWidth);
//        offset += halfOfWidth;
//    }
//    for (i = 0; i < halfOfHeight; i++)
//    {
//        memcpy(pDest + offset, pFrame->data[2] + i * pFrame->linesize[2], halfOfWidth);
//        offset += halfOfWidth;
//    }
//}
//void toYuv422p(char *pDest, AVFrame *pFrame)
//{
//    int index = 0;
//    int y_i = 0, u_i = 0, v_i = 0;
//    for (index = 0; index < pFrame->width * pFrame->height * 2;)
//    {
//        pDest[index++] = pFrame->data[0][y_i++];
//        pDest[index++] = pFrame->data[1][u_i++];
//        pDest[index++] = pFrame->data[0][y_i++];
//        pDest[index++] = pFrame->data[2][v_i++];
//    }
//}


