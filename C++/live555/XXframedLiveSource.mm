#include "XXframedLiveSource.h"

XXframedLiveSource* XXframedLiveSource::createNew(UsageEnvironment& env, GetLiveDataFunc getLiveDataFunc, void *userData, unsigned preferredFrameSize, unsigned playTimePerFrame)
{
    return new XXframedLiveSource(env, getLiveDataFunc, userData, preferredFrameSize, playTimePerFrame);
}

XXframedLiveSource::XXframedLiveSource(UsageEnvironment& env, GetLiveDataFunc getLiveDataFunc, void *userData, unsigned preferredFrameSize, unsigned playTimePerFrame)
    : FramedSource(env)
{
    _getLiveDataFunc    = getLiveDataFunc;
    _userData           = userData;
    
    _dataSizeMax    = 0;
    _dataSizeRead   = 0;
    _dataBuffer     = (uint8_t*)malloc(128*1024);
}
XXframedLiveSource::~XXframedLiveSource()
{
    delete _dataBuffer;
}

void XXframedLiveSource::doGetNextFrame(){
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
    }
    nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
    return;
}
