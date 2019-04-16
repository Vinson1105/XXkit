#include "XXfmUtility.h"

int xxfmUtility_openDecodeWithStream(AVCodecContext **codecContext, AVStream *stream){
    enum AVCodecID codecID = stream->codecpar->codec_id;
    AVCodec *codec = avcodec_find_decoder(codecID);
    if(NULL == codec){
        return -1;
    }

    AVCodecContext *context = avcodec_alloc_context3(codec);
    if(NULL == context){
        return -1;
    }

    if(avcodec_parameters_to_context(context, stream->codecpar) < 0){
        avcodec_free_context(&context);
        return -1;
    }

    // 通过codecpar转换之后不需要再设置timebase？？？
    //av_codec_set_pkt_timebase(videoDecodecContext, videoStream->time_base);
    if(0 != avcodec_open2(context, codec, NULL)){
        avcodec_free_context(&context);
        return -1;
    }

    *codecContext = context;
    return 0;
}
/*
int xxfmUtility_decode(AVCodecContext *decodeContext, AVFrame *frame, AVPacket *packet){
    if(avcodec_send_packet(decodeContext, packet) < 0){
        return -1;
    }

    int ret = 0;
    while (ret >= 0) {
        ret = avcodec_receive_frame(decodeContext, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;
        else if (ret < 0) {
            return -1;
        }
        else{

        }
    }

}
*/
int xxfmUtility_openEncodeWithParam(AVCodecContext **codecContext, enum AVCodecID codecID, enum AVPixelFormat format, int width, int height, int framerate, int bitrate, int gop){
    AVCodec *codec = avcodec_find_encoder(codecID);
    if(NULL == codec){
        return -1;
    }

    AVCodecContext *context = avcodec_alloc_context3(codec);
    if(NULL == context){
        return -1;
    }

    context->codec_id = codecID;
    context->codec_type = AVMEDIA_TYPE_VIDEO;
    context->pix_fmt = format;
    context->width = width;
    context->height = height;
    context->time_base = av_make_q(1,framerate);
    context->bit_rate = bitrate;
    context->gop_size = gop;

    //codecContext->qmin = 10;
    //codecContext->qmax = 51;
    // some formats want stream headers to be separate
    if (context->flags & AVFMT_GLOBALHEADER)
        context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    AVDictionary *options = 0;
    av_dict_set(&options, "preset", "superfast", 0);
    av_dict_set(&options, "tune", "zerolatency", 0);  //实现实时编码
    if (avcodec_open2(context, codec, &options) < 0){
        avcodec_free_context(&context);
        return -1;
    }

    *codecContext = context;
    return 0;
}
AVStream* xxfmUtility_findVideoStream(AVFormatContext *formatContext){
    for(int index = 0; index < formatContext->nb_streams; index++){
        if(AVMEDIA_TYPE_VIDEO == formatContext->streams[index]->codecpar->codec_type){
            return formatContext->streams[index];
        }
    }
    return NULL;
}
AVStream* xxfmUtility_findAudioStream(AVFormatContext *formatContext){
    for(int index = 0; index < formatContext->nb_streams; index++){
        if(AVMEDIA_TYPE_AUDIO == formatContext->streams[index]->codecpar->codec_type){
            return formatContext->streams[index];
        }
    }
    return NULL;
}

void xxfmUtility_transformTimeBase(AVFrame *frame, AVRational from, AVRational to){
    frame->pts = av_rescale_q(frame->pts, from, to);
    frame->pkt_pts = av_rescale_q(frame->pkt_pts, from, to);
    frame->pkt_dts = av_rescale_q(frame->pkt_dts, from, to);
    frame->pkt_duration = av_rescale_q(frame->pkt_duration, from, to);
}
void xxfmUtility_transformTimeBaseFromAnotherFrame(AVFrame *fromFrame,AVRational from, AVFrame *toFrame, AVRational to){
    toFrame->pts = av_rescale_q(fromFrame->pts, from, to);
    toFrame->pkt_pts = av_rescale_q(fromFrame->pkt_pts, from, to);
    toFrame->pkt_dts = av_rescale_q(fromFrame->pkt_dts, from, to);
    toFrame->pkt_duration = av_rescale_q(fromFrame->pkt_duration, from, to);
}