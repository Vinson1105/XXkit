//
//  H264LiveVideoServerMediaSubssion.hpp
//  XXapp
//
//  Created by VINSON on 2019/5/29.
//  Copyright © 2019 VINSON. All rights reserved.
//

#ifndef H264LiveVideoServerMediaSubssion_h
#define H264LiveVideoServerMediaSubssion_h

#include <liveMedia/H264VideoFileServerMediaSubsession.hh>

class H264LiveVideoServerMediaSubssion : public H264VideoFileServerMediaSubsession {
public:
    static H264LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env, Boolean reuseFirstSource, int *datasize, unsigned char*  databuf, bool *dosent);
    
protected: // we're a virtual base class
    H264LiveVideoServerMediaSubssion(UsageEnvironment& env, Boolean reuseFirstSource, int *datasize, unsigned char*  databuf, bool *dosent);
    ~H264LiveVideoServerMediaSubssion();
    
protected: // redefined virtual functions
    FramedSource* createNewStreamSource(unsigned clientSessionId,unsigned& estBitrate);
    
public:
    char fFileName[100];
    int *Server_datasize;//数据区大小指针
    unsigned char*  Server_databuf;//数据区指针
    bool *Server_dosent;//发送标示
};

#endif /* H264LiveVideoServerMediaSubssion_hpp */
