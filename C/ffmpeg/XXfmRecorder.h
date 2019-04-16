#ifndef XXfmDesktopRecorder_h
#define XXfmDesktopRecorder_h

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#include <libavutil/avutil.h>
#ifdef __cplusplus
    }
#endif /* __cplusplus */

typedef void* XXfmRecorderHandle;
typedef void (*XXfmRecorderGotVideoFunc)(void *opaque, int8_t *data, int length, uint64_t timestamp, int width, int height, bool isKeyFrame);
typedef void (*XXfmRecorderGotAudioFunc)(void *opaque, int8_t *data, int length, uint64_t timestamp);
typedef enum XXfmRecorderResult{
        XXfmRecorderResult_Finished = 1,
        XXfmRecorderResult_RequestInterruption = 0,

        XXfmRecorderResult_IllegalVideoAudioParam = -1,
        XXfmRecorderResult_FailureToFindInput = -2,
        XXfmRecorderResult_FailureToOpenInput = -3,
        XXfmRecorderResult_FailureToFindStream = -4,
        XXfmRecorderResult_FailureToOpenDecode = -5,
        XXfmRecorderResult_FailureToOpenEncode = -6,
        XXfmRecorderResult_FailureToGetSwsContext = -7,

        XXfmRecorderResult_FailureToReadFrame = -10,
        XXfmRecorderResult_FailureToSendPacket = -11,
        XXfmRecorderResult_FailureToReceiveFrame = -12,
        XXfmRecorderResult_FailureToScale = -13,
        XXfmRecorderResult_FailureToSendFrame = -14,
        XXfmRecorderResult_FailureToReceivePacket = -15,
}XXfmRecorderResult;
typedef void (*XXfmRecorderResultFunc)(void *opaque, XXfmRecorderResult result);

typedef struct XXfmRecorderVideoParam{
    char inputDevice[128];
    int desktopWidth;
    int desktopHeight;
    int desktopPixelFormat;

    int framerate;
    int width;
    int height;
}XXfmRecorderVideoParam;

typedef struct XXfmRecorderAudioParam{
    char inputDevice[128];
    int samplerate;
    int channel;
}XXfmRecorderAudioParam;

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

    XXfmRecorderHandle xxfmRecorder_create(void *opaque,
                                           XXfmRecorderGotVideoFunc gotVideoFunc,
                                           XXfmRecorderGotAudioFunc gotAudioFunc,
                                           XXfmRecorderResultFunc resultFunc,
                                           XXfmRecorderVideoParam *videoParam,
                                           XXfmRecorderAudioParam *audioParam);
    int xxfmRecorder_start(XXfmRecorderHandle handle);
    void xxfmRecorder_stop(XXfmRecorderHandle handle);
    void xxfmRecorder_free(XXfmRecorderHandle *handlePtr);

#ifdef __cplusplus
    }
#endif /* __cplusplus */

#endif /* XXfmDesktopRecorder_h */