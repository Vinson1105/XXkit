#include "XXl5StreamPusher.h"
#include "H264LiveVideoServerMediaSubssion.h"
#include "H265LiveVideoServerMediaSubssion.h"
#include "unistd.h"

static int getLiveData(void *userData, uint8_t *data, int *length){
    XXl5StreamPusher *pusher = (XXl5StreamPusher*)userData;
    pusher->getVideoDataInQueue(data, length);
    return 1;
}

XXl5StreamPusher::XXl5StreamPusher(XXavFrameType videoType, XXavFrameType audioType, int rtpPort, int rtcpPort, int rtspPort){
    _videoType  = videoType;
    _audioType  = audioType;
    _rtpPort    = rtpPort;
    _rtcpPort   = rtcpPort;
    _rtspPort   = rtspPort;
    
    _isInterruptionRequested    = false;
    _isRunning                  = false;
    _isInited                   = false;
    
    _videoQueueHandle   = NULL;//xxqueue_create(NULL);
    _audioQueueHandle   = xxqueue_create(NULL);
    
    _usageEnvironment   = NULL;
    _taskScheduler      = NULL;
    _rtspServer         = NULL;
    _rtcpInstance       = NULL;
    _h26xVideoRtpSink   = NULL;

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
    uint64_t *pts = (uint64_t*)(data->data + data->length - 8);
    _videoMutex.lock();
    if (NULL == _videoQueueHandle) {
        _videoMutex.unlock();
        return;
    }
    xxqueue_enqueue(_videoQueueHandle, data->data, (int)data->length);
    int count = xxqueue_count(_videoQueueHandle);
    printf("[%.3f] [Pusher] PTS:%.3f( + 队列数量:%d) (线程ID:%p)\n", (double)clock()/(CLOCKS_PER_SEC/10), (double)(*pts) / 1000.0, count, pthread_self());
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
        if (XXavFrameType_H265 ==  _videoType) {
            _h26xVideoRtpSink = H265VideoRTPSink::createNew(*_usageEnvironment, &rtpGroupsock, 96); // 96?
        }
        else{
            _h26xVideoRtpSink = H264VideoRTPSink::createNew(*_usageEnvironment, &rtpGroupsock, 96); // 96?
        }
    
        const uint32_t estimatedSessionBandwidth = 500;
        const uint32_t maxCHAMElen = 100;
        char CNAME[maxCHAMElen + 1];
        gethostname(CNAME, maxCHAMElen);
        CNAME[maxCHAMElen] = '\0';
        _rtcpInstance = RTCPInstance::createNew(*_usageEnvironment, &rtcpGroupsock, estimatedSessionBandwidth, (unsigned char*)CNAME, _h26xVideoRtpSink, NULL, true);
        increaseSendBufferTo(*_usageEnvironment, _h26xVideoRtpSink->groupsockBeingUsed().socketNum(), 128*1024);
        
        _rtspServer = RTSPServer::createNew(*_usageEnvironment, _rtspPort, NULL);
        if (NULL == _rtspServer) {
            return;
        }
        
        _serverMediaSession = ServerMediaSession::createNew(*_usageEnvironment);
    if (XXavFrameType_H265 == _videoType) {
        _h26xLiveVideoServerMediaSubssion = H265LiveVideoServerMediaSubssion::createNew(*_usageEnvironment, false, getLiveData, this);
    }
    else{
        _h26xLiveVideoServerMediaSubssion = H264LiveVideoServerMediaSubssion::createNew(*_usageEnvironment, false, getLiveData, this);
    }
        _serverMediaSession->addSubsession(_h26xLiveVideoServerMediaSubssion);
        _rtspServer->addServerMediaSession(_serverMediaSession);
    //    _isInited = true;
    //}

    char* url = _rtspServer->rtspURL(_serverMediaSession);
    printf("[XXl5StreamPusher] url >> %s\n", url);
    delete[] url;
    
    _usageEnvironment->taskScheduler().doEventLoop();
    
    _isRunning = false;
    _isInterruptionRequested = false;
}
void XXl5StreamPusher::closeSource(){
    _h26xVideoRtpSink->stopPlaying();
}
void XXl5StreamPusher::getVideoDataInQueue(uint8_t *data, int *length){
    _videoMutex.lock();
    if (NULL == _videoQueueHandle) {
        _videoQueueHandle = xxqueue_createWithLimit(NULL, 12, 1);//(NULL);
    }
    _videoMutex.unlock();
    
    while (1) {
        _videoMutex.lock();
        int count = xxqueue_count(_videoQueueHandle);
        if (count <= 0) {
            _videoMutex.unlock();
            usleep(10 * 1000);
            continue;
        }
        else{
            break;
        }
    }
    
    xxqueue_dequeue(_videoQueueHandle, data, length);
    int count = xxqueue_count(_videoQueueHandle);
    uint64_t *pts = (uint64_t*)(data + *length - 8);
    printf("[%.3f] [Pusher] PTS:%.3f( - 队列数量:%d) (线程ID:%p)\n", (double)clock()/(CLOCKS_PER_SEC/10), (double)(*pts) / 1000.0, count, pthread_self());
    _videoMutex.unlock();
    
    *length -= 8;
}

