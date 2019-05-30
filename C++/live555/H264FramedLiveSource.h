//
//  H264FramedLiveSource.hpp
//  XXapp
//
//  Created by VINSON on 2019/5/29.
//  Copyright © 2019 VINSON. All rights reserved.
//

#ifndef H264FramedLiveSource_h
#define H264FramedLiveSource_h

#include <liveMedia/FramedSource.hh>
typedef int(*GetLiveDataFunc)(void *userData, uint8_t *data, int *length);

class H264FramedLiveSource : public FramedSource{
public:
    //static H264FramedLiveSource* createNew(UsageEnvironment& env, unsigned char *frameData, int *frameDataSize,  bool *frameDataIsEnd, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);
    static H264FramedLiveSource* createNew(UsageEnvironment& env, GetLiveDataFunc getLiveDataFunc, void *userData, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);

protected:
    //H264FramedLiveSource(UsageEnvironment& env, unsigned char *frameData, int *frameDataSize,  bool *frameDataIsEnd, unsigned preferredFrameSize, unsigned playTimePerFrame);
    H264FramedLiveSource(UsageEnvironment& env, GetLiveDataFunc getLiveDataFunc, void *userData, unsigned preferredFrameSize, unsigned playTimePerFrame);
    ~H264FramedLiveSource();
    
private:
    virtual void doGetNextFrame();
    int TransportData(unsigned char* to, unsigned maxSize);
    
protected:
//    int *_frameDataSizeRef;         // 数据区大小指针
//    unsigned char *_frameDataRef;   // 数据区指针
//    bool *_frameDataIsEndRef;       // 发送标示
    
    GetLiveDataFunc _getLiveDataFunc;   // 获取数据回调
    void *_userData;                    // 回调参数
    
    int _dataSizeRead; // 记录已读取数据区大小
    int _dataSizeMax;  // 记录数据区大小
    uint8_t *_dataBuffer;   // 数据缓存
};

#endif /* H264FramedLiveSource_h */
