//
//  XXl5Pusher.hpp
//  XXapp
//
//  Created by VINSON on 2019/5/27.
//  Copyright © 2019 VINSON. All rights reserved.
//

#ifndef XXl5StreamPusher_h
#define XXl5StreamPusher_h

#include <stdio.h>
#include <mutex>
#include "../../XXutility.h"
#include "../../C/XXavUtility.h"
#include "../../C/XXqueue.h"

class XXl5StreamPusher {
public:
    XXl5StreamPusher(XXavFrameType videoType, XXavFrameType audioType, int rtpPort, int rtcpPort, int rtspPort);
    XXl5StreamPusher(const XXl5StreamPusher &pusher);
    virtual ~XXl5StreamPusher();
    
    bool isRunning();
    void start();
    void stop();
    void pushVideoData(const XXdata *data);
    void pushAudioData(const XXdata *data);
    
    void operator=(const XXl5StreamPusher &pusher);
    
private:
    
private:
    bool _isRunning;
    XXqueueHandle _videoQueueHandle;
    XXqueueHandle _audioQueueHandle;
};

#endif /* XXl5StreamPusher_h */
