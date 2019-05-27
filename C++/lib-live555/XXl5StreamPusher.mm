//
//  XXl5Pusher.cpp
//  XXapp
//
//  Created by VINSON on 2019/5/27.
//  Copyright © 2019 VINSON. All rights reserved.
//

#include "XXl5StreamPusher.h"
#include <thread>

XXl5StreamPusher::XXl5StreamPusher(XXavFrameType videoType, XXavFrameType audioType, int rtpPort, int rtcpPort, int rtspPort){
    _isRunning = false;
    _videoQueueHandle = NULL;
    _audioQueueHandle = NULL;
}
XXl5StreamPusher::XXl5StreamPusher(const XXl5StreamPusher &pusher){
    assert(1);
}
XXl5StreamPusher::~XXl5StreamPusher(){
    if (isRunning()) {
        stop();
    }
    
}

void XXl5StreamPusher::start(){
    
}
void XXl5StreamPusher::stop(){
    
}
void XXl5StreamPusher::pushVideoData(const XXdata *data){
    
}
void XXl5StreamPusher::pushAudioData(const XXdata *data){
    
}

void XXl5StreamPusher::operator=(const XXl5StreamPusher &pusher){
    assert(1);
}
