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
typedef int(*DoGetNextFrameFunc)(uint8_t **data, uint64_t *size);

class H264FramedLiveSource : public FramedSource{
public:
    static H264FramedLiveSource* createNew(UsageEnvironment& env, DoGetNextFrameFunc doGetNextFrameFunc, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);
    
protected:
    H264FramedLiveSource(UsageEnvironment& env, int *datasize, unsigned char *databuf, bool *dosent, unsigned preferredFrameSize, unsigned playTimePerFrame);
    ~H264FramedLiveSource();
    
private:
    virtual void doGetNextFrame();
    int TransportData(unsigned char* to, unsigned maxSize);
    
protected:
    int *Framed_datasize;           // 数据区大小指针
    unsigned char *Framed_databuf;  // 数据区指针
    bool *Framed_dosent;            // 发送标示
    
    int _dataSizeRead;   // 记录已读取数据区大小
    int _dataSizeMax;    // 记录数据区大小
};

#endif /* H264FramedLiveSource_h */
