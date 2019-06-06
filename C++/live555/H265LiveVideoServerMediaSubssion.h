//
//  H264LiveVideoServerMediaSubssion.hpp
//  XXapp
//
//  Created by VINSON on 2019/5/29.
//  Copyright © 2019 VINSON. All rights reserved.
//

#ifndef H265LiveVideoServerMediaSubssion_h
#define H265LiveVideoServerMediaSubssion_h

#include <liveMedia/H265VideoFileServerMediaSubsession.hh>
#include "XXframedLiveSource.h"

class H265LiveVideoServerMediaSubssion : public H265VideoFileServerMediaSubsession {
public:
    //static H264LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env, Boolean reuseFirstSource, unsigned char *frameData, int *frameDataSize,  bool *frameDataIsEnd);
    static H265LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env, Boolean reuseFirstSource, GetLiveDataFunc getLiveDataFunc, void *userData = NULL);
    
protected: // we're a virtual base class
    //H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource, unsigned char *frameData, int *frameDataSize,  bool *frameDataIsEnd);
    H265LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource, GetLiveDataFunc getLiveDataFunc, void *userData);
    ~H265LiveVideoServerMediaSubssion();
    
protected: // redefined virtual functions
    FramedSource* createNewStreamSource(unsigned clientSessionId,unsigned& estBitrate);
    
public:
    char fFileName[100];
    
//    int *_frameDataSizeRef;         // 数据区大小指针
//    unsigned char *_frameDataRef;   // 数据区指针
//    bool *_frameDataIsEndRef;       // 发送标示
    
    GetLiveDataFunc _getLiveDataFunc;   // 获取数据回调
    void *_userData;                    // 回调参数
};

#endif /* H264LiveVideoServerMediaSubssion_hpp */
