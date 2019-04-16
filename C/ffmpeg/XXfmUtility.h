#ifndef XXfmUtility_h
#define XXfmUtility_h

#ifdef __cplusplus
extern "C" {
#endif
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libavutil/audio_fifo.h>
    #include <libavutil/avutil.h>
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    int xxfmUtility_openDecodeWithStream(AVCodecContext **codecContex, AVStream *stream);
    int xxfmUtility_decode(AVCodecContext *decodeContext, AVFrame *frame, AVPacket *packet);

    int xxfmUtility_openEncodeWithParam(AVCodecContext **codecContext, enum AVCodecID codecID, enum AVPixelFormat format, int width, int height, int framerate, int bitrate, int gop);
    AVStream* xxfmUtility_findVideoStream(AVFormatContext *formatContext);
    AVStream* xxfmUtility_findAudioStream(AVFormatContext *formatContext);

    void xxfmUtility_transformTimeBase(AVFrame *frame, AVRational from, AVRational to);
    void xxfmUtility_transformTimeBaseFromAnotherFrame(AVFrame *fromFrame, AVRational from, AVFrame *toFrame, AVRational to);

#ifdef __cplusplus
}
#endif

#endif