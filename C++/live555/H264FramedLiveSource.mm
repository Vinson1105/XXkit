//
//  H264FramedLiveSource.cpp
//  XXapp
//
//  Created by VINSON on 2019/5/29.
//  Copyright © 2019 VINSON. All rights reserved.
//

#include "H264FramedLiveSource.h"

H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env, int *datasize, unsigned char*  databuf, bool *dosent, unsigned preferredFrameSize, unsigned playTimePerFrame){
    H264FramedLiveSource* newSource = new H264FramedLiveSource(env, datasize, databuf, dosent, preferredFrameSize, playTimePerFrame);
    return newSource;
}

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment &env, int *datasize, unsigned char*  databuf, bool *dosent, unsigned preferredFrameSize, unsigned playTimePerFrame)
    : FramedSource(env){
    Framed_datasize = datasize;//数据区大小指针
    Framed_databuf  = databuf;//数据区指针
    Framed_dosent   = dosent;//发送标示
}
H264FramedLiveSource::~H264FramedLiveSource()
{}

void H264FramedLiveSource::doGetNextFrame(){
    if (*Framed_dosent == true){
        *Framed_dosent = false;
        _dataSizeMax = *Framed_datasize;
        _dataSizeRead = 0;
        
        fFrameSize = fMaxSize;
        memcpy(fTo, Framed_databuf + _dataSizeRead, fFrameSize);
        _dataSizeRead += fFrameSize;
    }
    else{
        if (_dataSizeMax - _dataSizeRead>fMaxSize){
            fFrameSize = fMaxSize;
            memcpy(fTo, Framed_databuf + _dataSizeRead, fFrameSize);
            _dataSizeRead += fFrameSize;
        }
        else{
            fFrameSize= _dataSizeMax - _dataSizeRead;//fFrameSize是会告诉live555发送数据区的大小，必须赋予正确的值
            memcpy(fTo, Framed_databuf + _dataSizeRead, fFrameSize);
            *Framed_dosent = true;
        }
    }
    
    nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
    return;
}
