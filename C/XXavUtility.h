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
}XXavFrameType;

// 帧结构定义
typedef struct XXavFrame{
    char *data;         // 帧数据
    uint64_t length;    // 帧数据长度
    uint64_t dts;       // 帧解码时间戳
    uint64_t pts;       // 帧播放时间戳
    bool isVideo;       // 是否为视频帧
    bool isKey;         // 是否为关键帧
    XXavFrameType type; // 帧类型
}XXavFrame;

#endif // __XXavUtility_h
