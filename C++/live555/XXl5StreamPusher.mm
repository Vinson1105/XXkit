#include "XXl5StreamPusher.h"

static void afterVideoPlaying(void* clientData){
    printf("[XXl5StreamPusher] afterVideoPlaying\n");
}
static int getLiveData(void *userData, uint8_t *data, int *length){
    XXl5StreamPusher *pusher = (XXl5StreamPusher*)userData;
    pusher->getVideoDataInQueue(data, length);
    return 1;
}

XXl5StreamPusher::XXl5StreamPusher(XXavFrameType videoType, XXavFrameType audioType, int rtpPort, int rtcpPort, int rtspPort){
    _rtpPort    = rtpPort;
    _rtcpPort   = rtcpPort;
    _rtspPort   = rtspPort;
    
    _isInterruptionRequested    = false;
    _isRunning                  = false;
    _isInited                   = false;
    
    _videoQueueHandle   = xxqueue_create(NULL);//WithLimit(NULL, 12, 1);
    _audioQueueHandle   = xxqueue_create(NULL);
    
    _usageEnvironment   = NULL;
    _taskScheduler      = NULL;
    _rtspServer         = NULL;
    _rtcpInstance       = NULL;
    _h264VideoRtpSink   = NULL;

    _serverMediaSession = NULL;
    //_videoStreamFramer  = NULL;
    
    _videoBuffer    = NULL;
    _audioBuffer    = NULL;
}
XXl5StreamPusher::XXl5StreamPusher(const XXl5StreamPusher &pusher){
    assert(1);
}
XXl5StreamPusher::~XXl5StreamPusher(){
    if (_isRunning) {
        stop();
    }
    
    _videoMutex.lock();
    xxqueue_free(&_videoQueueHandle);
    _videoMutex.unlock();
    
    _audioMutex.lock();
    xxqueue_free(&_audioQueueHandle);
    _audioMutex.unlock();
    
    if (_videoBuffer) {
        free(_videoBuffer);
        _videoBuffer = NULL;
    }
    if (_audioBuffer) {
        free(_audioBuffer);
        _audioBuffer = NULL;
    }
}

void XXl5StreamPusher::start(){
    if (_isRunning || _isInterruptionRequested) {
        return;
    }
    
    _runThread = new std::thread(std::bind(&XXl5StreamPusher::run, this));
    _runThread->detach();
    
    _isRunning = true;
    printf("[XXl5StreamPusher] start\n");
}
void XXl5StreamPusher::stop(){
    if (!_isRunning) {
        return;
    }
    _isInterruptionRequested = true;
}
void XXl5StreamPusher::pushVideoData(const XXdata *data){
    _videoMutex.lock();
    xxqueue_enqueue(_videoQueueHandle, data->data, (int)data->length);
    int count = xxqueue_count(_videoQueueHandle);
    _videoMutex.unlock();
    
    //printf("[XXl5StreamPusher] [+] count:%d\n", count);
}
void XXl5StreamPusher::pushAudioData(const XXdata *data){
    _audioMutex.lock();
    xxqueue_enqueue(_audioQueueHandle, data->data, (int)data->length);
    _audioMutex.unlock();
}

void XXl5StreamPusher::operator=(const XXl5StreamPusher &pusher){
    assert(1);
}

void XXl5StreamPusher::run(){
    printf("[XXl5StreamPusher] run (%d)\n", getpid());

    //if (!_isInited) {
        // 创建时间表
        _taskScheduler = BasicTaskScheduler::createNew();
        // 创建使用的环境
        _usageEnvironment = BasicUsageEnvironment::createNew(*_taskScheduler);
        
        struct in_addr destinationAddress;
        destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*_usageEnvironment);
        
        // rtp和rtcp端口
        const uint8_t ttl = 255;
        const Port rtpPort(_rtpPort);
        const Port rtcpPort(_rtcpPort);
        
        // 创建rtp和rtcp收发socket
        Groupsock rtpGroupsock(*_usageEnvironment, destinationAddress, rtpPort, ttl);
        rtpGroupsock.multicastSendOnly();
        Groupsock rtcpGroupsock(*_usageEnvironment, destinationAddress, rtcpPort, ttl);
        rtpGroupsock.multicastSendOnly();
        
        OutPacketBuffer::maxSize = 128*1024;
        _h264VideoRtpSink = H264VideoRTPSink::createNew(*_usageEnvironment, &rtpGroupsock, 96); // 96?
    
        const uint32_t estimatedSessionBandwidth = 500;
        const uint32_t maxCHAMElen = 100;
        char CNAME[maxCHAMElen + 1];
        gethostname(CNAME, maxCHAMElen);
        CNAME[maxCHAMElen] = '\0';
        _rtcpInstance = RTCPInstance::createNew(*_usageEnvironment, &rtcpGroupsock, estimatedSessionBandwidth, (unsigned char*)CNAME, _h264VideoRtpSink, NULL, true);
        
        _rtspServer = RTSPServer::createNew(*_usageEnvironment, _rtspPort, NULL);
        if (NULL == _rtspServer) {
            return;
        }
        
        _serverMediaSession = ServerMediaSession::createNew(*_usageEnvironment);
        _h264liveVideoServerMediaSubssion = H264LiveVideoServerMediaSubssion::createNew(*_usageEnvironment, false, getLiveData, this);
        _serverMediaSession->addSubsession(_h264liveVideoServerMediaSubssion);
        _rtspServer->addServerMediaSession(_serverMediaSession);
    //    _isInited = true;
    //}
    
    char* url = _rtspServer->rtspURL(_serverMediaSession);
    printf("[XXl5StreamPusher] url >> %s\n", url);
    delete[] url;
    
    //sendVideoFrame();
    //H264FramedLiveSource *liveSoucre = H264FramedLiveSource::createNew(*_usageEnvironment, getLiveData, this);
    //_h264VideoRtpSink->startPlaying(*liveSoucre, afterVideoPlaying, this);
    _usageEnvironment->taskScheduler().doEventLoop();
    
    _isRunning = false;
    _isInterruptionRequested = false;
}
void XXl5StreamPusher::closeSource(){
    _h264VideoRtpSink->stopPlaying();
}
void XXl5StreamPusher::getVideoDataInQueue(uint8_t *data, int *length){
    //printf("[XXl5StreamPusher] getVideoDataInQueue\n");
    _videoMutex.lock();
    xxqueue_dequeue(_videoQueueHandle, data, length);
    _videoMutex.unlock();
}

