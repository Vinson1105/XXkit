//
//  H264FramedLiveSource.cpp
//  XXapp
//
//  Created by VINSON on 2019/5/29.
//  Copyright © 2019 VINSON. All rights reserved.
//

#include "H264FramedLiveSource.h"

//H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env, unsigned char *frameData, int *frameDataSize,  bool *frameDataIsEnd, unsigned preferredFrameSize, unsigned playTimePerFrame){
//    H264FramedLiveSource* newSource = new H264FramedLiveSource(env, frameData, frameDataSize, frameDataIsEnd, preferredFrameSize, playTimePerFrame);
//    return newSource;
//}
H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env, GetLiveDataFunc getLiveDataFunc, void *userData, unsigned preferredFrameSize, unsigned playTimePerFrame)
{
    return new H264FramedLiveSource(env, getLiveDataFunc, userData, preferredFrameSize, playTimePerFrame);
}


//H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment &env, unsigned char *frameData, int *frameDataSize,  bool *frameDataIsEnd, unsigned preferredFrameSize, unsigned playTimePerFrame)
//    : FramedSource(env){
//    _frameDataSizeRef       = frameDataSize;    // 数据区大小指针
//    _frameDataRef           = frameData;        // 数据区指针
//    _frameDataIsEndRef      = frameDataIsEnd;   // 发送标示
//}
H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment& env, GetLiveDataFunc getLiveDataFunc, void *userData, unsigned preferredFrameSize, unsigned playTimePerFrame)
    : FramedSource(env)
{
    _getLiveDataFunc    = getLiveDataFunc;
    _userData           = userData;
    
    _dataSizeMax    = 0;
    _dataSizeRead   = 0;
    _dataBuffer     = (uint8_t*)malloc(128*1024);
}
H264FramedLiveSource::~H264FramedLiveSource()
{
    delete _dataBuffer;
}

void H264FramedLiveSource::doGetNextFrame(){
    if (0 == _dataSizeMax) {
        _getLiveDataFunc(_userData, _dataBuffer, &_dataSizeMax);
    }
    if (_dataSizeMax > 0) {
        if (_dataSizeMax - _dataSizeRead > fMaxSize) {
            memcpy(fTo, _dataBuffer + _dataSizeRead, fMaxSize);
            fFrameSize      = fMaxSize;
            _dataSizeRead   += fMaxSize;
        }
        else{
            fFrameSize = _dataSizeMax - _dataSizeRead;
            memcpy(fTo, _dataBuffer + _dataSizeRead, fFrameSize);
            _dataSizeMax = 0;
            _dataSizeRead = 0;
        }
        printf("[H264FramedLiveSource] send:%d all:%d\n", _dataSizeRead, _dataSizeMax);
        nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
    }
    else{
        nextTask() = envir().taskScheduler().scheduleDelayedTask(10, (TaskFunc*)FramedSource::afterGetting, this);
    }
    return;
}
//void H264FramedLiveSource::doGetNextFrame(){
//    if (*_frameDataIsEndRef == true){
//        *_frameDataIsEndRef = false;
//        _dataSizeMax = *_frameDataSizeRef;
//        _dataSizeRead = 0;
//
//        fFrameSize = fMaxSize;
//        memcpy(fTo, _frameDataRef + _dataSizeRead, fFrameSize);
//        _dataSizeRead += fFrameSize;
//    }
//    else{
//        if (_dataSizeMax - _dataSizeRead>fMaxSize){
//            fFrameSize = fMaxSize;
//            memcpy(fTo, _frameDataRef + _dataSizeRead, fFrameSize);
//            _dataSizeRead += fFrameSize;
//        }
//        else{
//            fFrameSize= _dataSizeMax - _dataSizeRead;//fFrameSize是会告诉live555发送数据区的大小，必须赋予正确的值
//            memcpy(fTo, _frameDataRef + _dataSizeRead, fFrameSize);
//            *_frameDataIsEndRef = true;
//        }
//    }
//
//    nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
//    return;
//}
