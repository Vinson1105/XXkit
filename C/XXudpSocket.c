//
//  XXudpSocket.c
//  NVSDK
//
//  Created by VINSON on 2019/8/23.
//  Copyright © 2019 macrovideo. All rights reserved.
//

#include "XXudpSocket.h"
#include "XXnet.h"
#include <stdlib.h>
#include <unistd.h>

#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>

#include <netdb.h>
#include <errno.h>

#define DECLARE_CONTEXT(handle) XXudpSocketContext *context = (XXudpSocketContext*)(handle);

static void msecToTimeVal(struct timeval *time, time_t msec);
static void resetFDset(int socket, fd_set *fdset);
static bool loopToSelect(int socket, int times, struct timeval *timeval, fd_set *fdread);

typedef struct XXudpSocketContext{
    fd_set  fdwrite;
    fd_set  fdread;
    int     socket;
    int     recvTryTimes;
    int     recvBufferSize;
    struct  timeval selectTime;
    
    char    *addressArray[5];
}XXudpSocketContext;

XXudpSocketHandle xxudpSocket_alloc(int recvBufferSize, int recvTryTimes, time_t waitSelectMsec){
    XXudpSocketContext *context = (XXudpSocketContext*)malloc(sizeof(XXudpSocketContext));
    
    context->socket             = socket(AF_INET, SOCK_DGRAM, 0);
    context->recvTryTimes       = recvTryTimes;
    context->recvBufferSize     = recvBufferSize;
    msecToTimeVal(&(context->selectTime), waitSelectMsec);
    
    return context;
}
int xxudpSocket_bind(XXudpSocketHandle handle, const char *host, int port, bool reusdPort){
    DECLARE_CONTEXT(handle)
    if (NULL == context) {
        return -1;
    }
    if (context->socket <= 0) {
        return -2;
    }
    
    struct sockaddr_in address;
    bzero(&address, sizeof(struct sockaddr_in));
    address.sin_family       = AF_INET;
    address.sin_port         = htons(port);
    address.sin_addr.s_addr  = NULL == host ? htonl(INADDR_ANY) : inet_addr(host);
    
    int on = reusdPort ? 1 : 0;
    setsockopt(context->socket, SOL_SOCKET, SO_REUSEPORT, (void*)&on, sizeof(on));
    if(0 != bind(context->socket, (struct sockaddr*)&address, sizeof(struct sockaddr_in))){
        printf("[XXudpSocket] 端口绑定失败 (line:%d | errno:%d)", __LINE__, errno);
        return -3;
    }
    
    return 0;
}
ssize_t xxudpSocket_sendto(XXudpSocketHandle handle, const char *host, int port, uint8_t *data, ssize_t length){
    DECLARE_CONTEXT(handle)
    if (NULL == context) {
        return -1;
    }
    if (context->socket <= 0) {
        return -2;
    }
    
    char realHost[128] = {0};
    bool isIPv6 = false;
    if(!xxnet_transformHost(realHost, &isIPv6, (char*)host, port)){
        printf("[XXudpSocket] 地址转换失败 (line:%d | errno:%d)", __LINE__, errno);
        return -3;
    }
    
    struct sockaddr_in address4;
    struct sockaddr_in6 address6;
    struct sockaddr *address = NULL;
    socklen_t addressLength = 0;
    if(isIPv6){
        xxnet_address6(&address6, realHost, port);
        address = (struct sockaddr*)&address6;
        addressLength = sizeof(struct sockaddr_in6);
    }
    else{
        xxnet_address4(&address4, realHost, port);
        address = (struct sockaddr*)&address4;
        addressLength = sizeof(struct sockaddr_in);
    }
    
    resetFDset(context->socket, &context->fdwrite);
    if (select(context->socket + 1, NULL, &(context->fdwrite), NULL, &(context->selectTime)) <= 0){
        return -4;
    }
    
    return sendto(context->socket, data, length, 0, address, addressLength);
}
ssize_t xxudpSocket_recvFrom(XXudpSocketHandle handle, uint8_t *data, ssize_t length, char *srcHost, int *srcPort, bool *isIPv6Src){
    DECLARE_CONTEXT(handle)
    if (NULL == context) {
        return -1;
    }
    if (context->socket <= 0) {
        return -2;
    }
    if ( !loopToSelect(context->socket, context->recvTryTimes, &(context->selectTime), &(context->fdread)) ) {
        return -3;
    }
    
    uint8_t address[32] = {0};
    struct sockaddr *socketAddress = (struct sockaddr *)address;
    socklen_t socketLength = 0;
    ssize_t ret = recvfrom(context->socket, data, length, 0, socketAddress, &socketLength);
    if (ret <= 0) {
        return -4;
    }
    
    xxnet_infoFromAddress(srcHost, srcPort, isIPv6Src, socketAddress);
    return ret;
}
void xxudpSocket_free(XXudpSocketHandle **handlePtr){
    if (NULL == handlePtr && NULL == *handlePtr) {
        return;
    }
    DECLARE_CONTEXT(*handlePtr)
    close(context->socket);
    free(context);
    *handlePtr = NULL;
}

//
//
//
static void msecToTimeVal(struct timeval *time, time_t msec){
    time->tv_usec   = msec % 1000 * 1000;
    time->tv_sec    = msec / 1000;
}
static void resetFDset(int socket, fd_set *fdset){
    FD_ZERO(fdset);
    FD_SET(socket, fdset);
}
static bool loopToSelect(int socket, int times, struct timeval *timeval, fd_set *fdread){
    for (int index = 0; index < times; index++) {
        resetFDset(socket, fdread);
        
        int ret = select(socket + 1, fdread, NULL, NULL, timeval);
        if (ret < 0)        { return false; }
        else if(0 == ret)   { continue; }
        else                { return true; }
    }
    return false;
}
