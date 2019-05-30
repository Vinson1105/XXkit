//
//  H264LiveVideoServerMediaSubssion.cpp
//  XXapp
//
//  Created by VINSON on 2019/5/29.
//  Copyright © 2019 VINSON. All rights reserved.
//

#include "H264LiveVideoServerMediaSubssion.h"
#include "H264FramedLiveSource.h"
#include <liveMedia/H264VideoStreamFramer.hh>

//H264LiveVideoServerMediaSubssion* H264LiveVideoServerMediaSubssion::createNew(UsageEnvironment& env, Boolean reuseFirstSource, unsigned char *frameData, int *frameDataSize,  bool *frameDataIsEnd){
//    return new H264LiveVideoServerMediaSubssion(env, reuseFirstSource, frameData, frameDataSize, frameDataIsEnd);
//}
H264LiveVideoServerMediaSubssion* H264LiveVideoServerMediaSubssion::createNew(UsageEnvironment& env, Boolean reuseFirstSource, GetLiveDataFunc getLiveDataFunc, void *userData){
    return new H264LiveVideoServerMediaSubssion(env, reuseFirstSource, getLiveDataFunc, userData);
}

//H264LiveVideoServerMediaSubssion::H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource, unsigned char *frameData, int *frameDataSize,  bool *frameDataIsEnd)
//    : H264VideoFileServerMediaSubsession(env, fFileName, reuseFirstSource)  //H264VideoFileServerMediaSubsession不是我们需要修改的文件，但是我们又要用它来初始化我们的函数，所以给个空数组进去即可
//{
//    _frameDataRef       = frameData;//数据区大小指针
//    _frameDataSizeRef   = frameDataSize;//数据区指针
//    _frameDataIsEndRef  = frameDataIsEnd;//发送标示
//}
H264LiveVideoServerMediaSubssion::H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource, GetLiveDataFunc getLiveDataFunc, void *userData)
    : H264VideoFileServerMediaSubsession(env, fFileName, reuseFirstSource)
{
    _getLiveDataFunc    = getLiveDataFunc;
    _userData           = userData;
}
H264LiveVideoServerMediaSubssion::~H264LiveVideoServerMediaSubssion()
{}

FramedSource* H264LiveVideoServerMediaSubssion::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate){
    /* Remain to do : assign estBitrate */
    estBitrate = 1000; // kbps, estimate
    
    //创建视频源
    H264FramedLiveSource* liveSource = H264FramedLiveSource::createNew(envir(), _getLiveDataFunc, _userData);
    if (liveSource == NULL){
        return NULL;
    }
    
    // Create a framer for the Video Elementary Stream:
    return H264VideoStreamFramer::createNew(envir(), liveSource);
}
