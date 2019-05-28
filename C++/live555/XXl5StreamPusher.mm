#include "XXl5StreamPusher.h"
const int VIDEO_BUFFER_SIZE_MAX = 128*1024;
const int AUDIO_BUFFER_SIZE_MAX = 16*1024;

static void afterVideoPlaying(void* clientData){
    XXl5StreamPusher *pusher = (XXl5StreamPusher*)clientData;
    pusher->sendVideoFrame();
}
static void startStream(XXl5StreamPusher *pusher){
    pusher->run();
}

XXl5StreamPusher::XXl5StreamPusher(XXavFrameType videoType, XXavFrameType audioType, int rtpPort, int rtcpPort, int rtspPort){
    _rtpPort    = rtpPort;
    _rtcpPort   = rtcpPort;
    _rtspPort   = rtspPort;
    
    _isInterruptionRequested = false;
    _isRunning          = false;
    _videoQueueHandle   = xxqueue_create(-1, NULL);
    _audioQueueHandle   = xxqueue_create(-1, NULL);
    
    _usageEnvironment   = NULL;
    _taskScheduler      = NULL;
    _h264VideoRtpSink   = NULL;
    _rtspServer         = NULL;
    
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
}
void XXl5StreamPusher::stop(){
    _isInterruptionRequested = true;
}
void XXl5StreamPusher::pushVideoData(const XXdata *data){
    _videoMutex.lock();
    xxqueue_enqueue(_videoQueueHandle, data->data, data->length);
    int count = xxqueue_count(_videoQueueHandle);
    _videoMutex.unlock();
    
    printf("[XXl5StreamPusher] [+] count:%d\n", count);
}
void XXl5StreamPusher::pushAudioData(const XXdata *data){
    _audioMutex.lock();
    xxqueue_enqueue(_audioQueueHandle, data->data, data->length);
    _audioMutex.unlock();
}

void XXl5StreamPusher::operator=(const XXl5StreamPusher &pusher){
    assert(1);
}

void XXl5StreamPusher::run(){
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
    
    OutPacketBuffer::maxSize = 100000;
    _h264VideoRtpSink = H264VideoRTPSink::createNew(*_usageEnvironment, &rtpGroupsock, 96); // 96?
    
    const uint32_t estimatedSessionBandwidth = 500;
    const uint32_t maxCHAMElen = 100;
    char CNAME[maxCHAMElen + 1];
    gethostname(CNAME, maxCHAMElen);
    CNAME[maxCHAMElen] = '\0';
    RTCPInstance *rtcp = RTCPInstance::createNew(*_usageEnvironment, &rtcpGroupsock, estimatedSessionBandwidth, (unsigned char*)CNAME, _h264VideoRtpSink, NULL, true);
    
    _rtspServer = RTSPServer::createNew(*_usageEnvironment, _rtspPort, NULL);
    if (NULL == _rtspServer) {
        return;
    }
    
    ServerMediaSession *serverMediaSession = ServerMediaSession::createNew(*_usageEnvironment, "XXappStream");
    serverMediaSession->addSubsession(PassiveServerMediaSubsession::createNew(*_h264VideoRtpSink, rtcp));
    _rtspServer->addServerMediaSession(serverMediaSession);
    
    char* url = _rtspServer->rtspURL(serverMediaSession);
    *_usageEnvironment << "Play this stream using the URL \"" << url << "\"\n";
    delete[] url;
    
    sendVideoFrame();
    _usageEnvironment->taskScheduler().doEventLoop();
    
    _isRunning = false;
    _isInterruptionRequested = false;
}
void XXl5StreamPusher::sendVideoFrame(){
    _videoMutex.lock();
    if (!_isRunning || _isInterruptionRequested) {
        _videoMutex.unlock();
        return ;
    }
    
    if (NULL == _videoQueueHandle) {
        _videoMutex.unlock();
        return ;
    }
    
    int count = xxqueue_count(_videoQueueHandle);
    if (count <= 0) {
        _videoMutex.unlock();
        //return ;
    }
    
    if (NULL == _videoBuffer) {
        _videoBuffer = (uint8_t*)malloc(VIDEO_BUFFER_SIZE_MAX);
    }
    uint64_t length = 0;
    int ret = xxqueue_dequeue(_videoQueueHandle, _videoBuffer, &length);
    count = xxqueue_count(_videoQueueHandle);
    _videoMutex.unlock();
    if(ret < 0){
        printf("[XXl5StreamPusher] [LINE:%d] could not dequeue video data.\n", __LINE__);
        return;
    }
    else if(XXQUEUE_IS_EMPTY == ret){
        printf("[XXl5StreamPusher] [-] count:%d\n", count);
    }
    else{
        printf("[XXl5StreamPusher] [-] count:%d\n", count);
    }
    
    ByteStreamMemoryBufferSource *bufferSoucre = ByteStreamMemoryBufferSource::createNew(*_usageEnvironment, _videoBuffer, length, false);
    if (NULL == bufferSoucre) {
        printf("[XXl5StreamPusher] [LINE:%d] could not create new video buffer source.\n", __LINE__);
        return;
    }
    H264VideoStreamFramer *streamFramer = H264VideoStreamFramer::createNew(*_usageEnvironment, bufferSoucre, false);
    _h264VideoRtpSink->startPlaying(*streamFramer, afterVideoPlaying, this);
}
void XXl5StreamPusher::sendAudioFrame(){
    
}
