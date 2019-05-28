#ifndef XXl5StreamPusher_h
#define XXl5StreamPusher_h

#include <stdio.h>
#include <mutex>
#include <thread>

#include "../../XXutility.h"
#include "../../C/XXavUtility.h"
#include "../../C/XXqueue.h"

#include <liveMedia/liveMedia.hh>
#include <BasicUsageEnvironment/BasicUsageEnvironment.hh>
#include <UsageEnvironment/UsageEnvironment.hh>
#include <groupsock/GroupsockHelper.hh>

class XXl5StreamPusher {
public:
    XXl5StreamPusher(XXavFrameType videoType, XXavFrameType audioType, int rtpPort, int rtcpPort, int rtspPort);
    XXl5StreamPusher(const XXl5StreamPusher &pusher);
    virtual ~XXl5StreamPusher();
    
    void start();
    void stop();
    void pushVideoData(const XXdata *data);
    void pushAudioData(const XXdata *data);
    
    void operator=(const XXl5StreamPusher &pusher);
    
public:
    void sendVideoFrame();
    void sendAudioFrame();
    void run();
    
private:
    int _rtpPort;
    int _rtcpPort;
    int _rtspPort;
    
    bool _isInterruptionRequested;
    bool _isRunning;
    std::mutex _videoMutex;
    std::mutex _audioMutex;
    XXqueueHandle _videoQueueHandle;
    XXqueueHandle _audioQueueHandle;
    
    UsageEnvironment *_usageEnvironment;
    TaskScheduler *_taskScheduler;
    H264VideoRTPSink *_h264VideoRtpSink;
    RTSPServer *_rtspServer;
    
    uint8_t *_videoBuffer;
    uint8_t *_audioBuffer;
    std::thread *_runThread;
};

#endif /* XXl5StreamPusher_h */
