/**
 * @author: GWX
 * @date: 20190319
 * @descirption:
 *      1.以AVCC(264)或者HVCC(265)格式合成MP4
 *      2.所有函数线程不安全
 * @history:
 *  GWX - 20190321
 *  1.close接口增加是否保存文件参数
 *  2.优化部分逻辑
 */
#ifndef XXfmMp4Writer_h
#define XXfmMp4Writer_h

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "../XXavDefine.h"

// 视频参数
typedef struct XXfmMp4WriterVideoParam{
    XXavFrameType type;
    int width;
    int height;
    int frameRate;
}XXfmMp4WriterVideoParam;

// 音频采样类型(与ffmpeg的SampleFormat对应,请不要随意修改)
typedef enum XXfmMp4WriterAudioSampleFormat{
    XXfmMp4WriterAudioSampleFormat_NONE = -1,
    
    XXfmMp4WriterAudioSampleFormat_U8,          ///< unsigned 8 bits
    XXfmMp4WriterAudioSampleFormat_S16,         ///< signed 16 bits
    XXfmMp4WriterAudioSampleFormat_S32,         ///< signed 32 bits
    XXfmMp4WriterAudioSampleFormat_FLT,         ///< float
    XXfmMp4WriterAudioSampleFormat_DBL,         ///< double
    
    XXfmMp4WriterAudioSampleFormat_U8P,         ///< unsigned 8 bits, planar
    XXfmMp4WriterAudioSampleFormat_S16P,        ///< signed 16 bits, planar
    XXfmMp4WriterAudioSampleFormat_S32P,        ///< signed 32 bits, planar
    XXfmMp4WriterAudioSampleFormat_FLTP,        ///< float, planar
    XXfmMp4WriterAudioSampleFormat_DBLP,        ///< double, planar
    
    XXfmMp4WriterAudioSampleFormat_NB
}XXfmMp4WriterAudioSampleFormat;

// 音频参数
typedef struct XXfmMp4WriterAudioParam{
    XXavFrameType type;
    int channelCount;
    int sampleRate;
    XXfmMp4WriterAudioSampleFormat sampleFormat;
}XXfmMp4WriterAudioParam;

// 句柄
typedef void* XXfmMp4WriterHandle;

#ifdef __cplusplus
extern "C" {
#endif
    /**
     * @brief   创建句柄,注意如果video和audio的时间戳不为毫秒级别的,则使用video的framerate和audio的samplerate计算时间戳
     * @param   path 目标MP4文件路径
     * @param   videoParam 视频对应参数
     * @param   audioParam 音频对应参数
     * @return  返回NULL失败,否则成功
     */
    XXfmMp4WriterHandle xxfmMp4Writer_create(char *path, XXfmMp4WriterVideoParam *videoParam, XXfmMp4WriterAudioParam *audioParam);
    
    /**
     * @brief   写入视频帧数据,暂不能处理分片过的数据
     * @param   handle 对应句柄
     * @param   data 视频数据
     * @param   length 数据长度
     * @param   timestamp 时间戳,注意需要是毫秒级别,否则请置为0
     * @param   isKeyFrame 是否为关键帧
     * @return  返回>=0成功,否则失败
     */
    int xxfmMp4Writer_video(XXfmMp4WriterHandle handle, uint8_t *data, int length, int64_t timestamp, bool isKeyFrame);
    
    /**
     * @brief   写入音频帧数据
     * @param   handle 对应句柄
     * @param   data 音频数据
     * @param   length 数据长度
     * @param   timestamp 时间戳,注意需要是毫秒级别,否则请置为0
     * @return  返回>=0成功,否则失败
     */
    int xxfmMp4Writer_audio(XXfmMp4WriterHandle handle, uint8_t *data, int length, int64_t timestamp);
    
    /**
     * @brief   关闭并释放资源,并把handle置为NULL
     * @param   handle 对应句柄
     * @param   save 指示对应的文件是否需要保存,true:保存,false:不保存
     */
    void xxfmMp4Writer_close(XXfmMp4WriterHandle *handle, bool save);
#ifdef __cplusplus
}
#endif
#endif /* XXfmMp4Writer_h */
