#ifndef XXframedLiveSource_h
#define XXframedLiveSource_h

#include <liveMedia/FramedSource.hh>
typedef int(*GetLiveDataFunc)(void *userData, uint8_t *data, int *length);

class XXframedLiveSource : public FramedSource{
public:
    static XXframedLiveSource* createNew(UsageEnvironment& env, GetLiveDataFunc getLiveDataFunc, void *userData, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);

protected:
    XXframedLiveSource(UsageEnvironment& env, GetLiveDataFunc getLiveDataFunc, void *userData, unsigned preferredFrameSize, unsigned playTimePerFrame);
    ~XXframedLiveSource();
    
private:
    virtual void doGetNextFrame();
    int TransportData(unsigned char* to, unsigned maxSize);
    
protected:
    GetLiveDataFunc _getLiveDataFunc;   // 获取数据回调
    void *_userData;                    // 回调参数
    
    int _dataSizeRead; // 记录已读取数据区大小
    int _dataSizeMax;  // 记录数据区大小
    uint8_t *_dataBuffer;   // 数据缓存
};

#endif /* XXframedLiveSource_h */
