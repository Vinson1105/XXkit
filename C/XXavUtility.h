/*
 * @author: GWX
 * @date: 20181229
 * @descirption: 定义此工具库中通用音视频结构
 * @history:
 */

#ifndef XXavUtility_h
#define XXavUtility_h

#include <stdbool.h>
#include <stdint.h>

// 帧类型定义
typedef enum XXavFrameType{
    XXavFrameType_Unknown = 1,
    XXavFrameType_H264,
    XXavFrameType_H265,
    XXavFrameType_AAC,
    
    XXavFrameType_YUV420,
    XXavFrameType_PCM,
}XXavFrameType;

// 帧结构定义
typedef struct XXavFrame{
    int8_t *data;         // 帧数据
    uint64_t length;    // 帧数据长度
    uint64_t dts;       // 帧解码时间戳
    uint64_t pts;       // 帧播放时间戳
    bool isVideo;       // 是否为视频帧
    bool isKey;         // 是否为关键帧
    XXavFrameType type; // 帧类型
}XXavFrame;

typedef void (*XXavFrameCallBack)(void *opaque, XXavFrame *frame);


#ifdef __cplusplus
extern "C" {
#endif
    XXavFrame* xxavFrame_alloc(void);
    void xxavFrame_free(XXavFrame **frame);    
#ifdef __cplusplus
}
#endif

#endif // __XXavUtility_h
