//
//  XXudpSocket.c
//  teset
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 XX. All rights reserved.
//

#include "XXudpSocket.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef struct XXudpSocketContext {
    fd_set  fdwrite;
    fd_set  fdread;
    int socket;
    int tryTimes;
    int recvBufferSize;
    struct  timeval selectTime;
    bool binded;
    
    bool    hasSomething;
    int     recvCountAfterSelect;
    
    char    *addressArray[5];
}XXudpSocketContext;

// 私有函数声明 {
static void msecToTimeVal(struct timeval *time, time_t msec);
static ssize_t loopToSend(int socket, int8_t *data, ssize_t length, int times, struct timeval *timeval, fd_set *fdwrite, const char *ip, int port);
static ssize_t loopToRecv(int socket, int8_t *data, ssize_t maxLength, int times, struct timeval *timeval, fd_set *fdread, bool *hasSomething, int *recvCountAfterSelect);

//static int lookupHost(const char *host, uint16_t port, char** addressArray, int *addressCapacity);
//static bool isIPV6Address(char *address);
//static bool hostFromAddress(char *hostBuffer, char *address);
//static bool getHost(char *hostBuffer, uint16_t *port, sa_family_t *family, char *address);
//
//static bool hostFromSockaddr4(char *hostBuffer, const struct sockaddr_in *sockaddr4);
//static bool hostFromSockaddr6(char *hostBuffer, const struct sockaddr_in6 *sockaddr6);
//static uint16_t portFromSockaddr4(const struct sockaddr_in *sockaddr4);
//static uint16_t portFromSockaddr6(const struct sockaddr_in6 *sockaddr6);
// } 私有函数声明

XXudpSocketHandle xxudpSocket_alloc(int recvBufferSize, int tryTimes, time_t waitSelectMsec){
    int newSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(newSocket<=0){
        return NULL;
    }
    
    const int on = 1;
    
    // 开启地址端口重用，但是如果之前有绑定同一个地址端口的socket没有开启重用，则后续的socket不管有没有设置重用，都不能成功绑定
    setsockopt(newSocket, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    // 防止socket所在的线程关闭，该线程的socket没有并没有及时关闭，导致的崩溃，一般建议socket在线程中要及时回收
    setsockopt(newSocket, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));
    
    // 设置socket广播权限
    setsockopt(newSocket, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    
    XXudpSocketContext *context = (XXudpSocketContext*)malloc(sizeof(XXudpSocketContext));
    context->tryTimes = tryTimes;
    context->socket = newSocket;
    context->recvBufferSize = recvBufferSize;
    context->binded = false;
    msecToTimeVal(&(context->selectTime), waitSelectMsec);

    context->hasSomething       = false;
    context->recvCountAfterSelect = 0;
    
    for (int i = 0; i<5; i++) {
        char *buffer = malloc(128);
        bzero(buffer, 128);
        context->addressArray[i] = buffer;
    }
    return context;
}
int xxudpSocket_bind(XXudpSocketHandle handle, int port){
    if(NULL == handle){
        return -1;
    }
    XXudpSocketContext *context = handle;
    if(context->binded){
        return -1;
    }
        
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr  = htonl (INADDR_ANY);

    int ret = bind(context->socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    if(0 != ret){
        return ret;
    }
    context->binded = true;
    
    
    return 1;
}
ssize_t xxudpSocket_send(XXudpSocketHandle handle, const char *ip, int port, int8_t *data, ssize_t length){
    XXudpSocketContext *context = handle;
    if(NULL == context || !context->binded){
        return -1;
    }

    return loopToSend(context->socket, data, length, context->tryTimes, &(context->selectTime), &(context->fdwrite), ip, port);
}
ssize_t xxudpSocket_recv(XXudpSocketHandle handle, int8_t *data, ssize_t length){
    XXudpSocketContext *context = handle;
    if(NULL == context || !context->binded){
        return -1;
    }
    return loopToRecv(context->socket, data, length, context->tryTimes, &(context->selectTime), &(context->fdread), &(context->hasSomething), &(context->recvCountAfterSelect));
}
void xxudpSocket_free(XXudpSocketHandle *handlePtr){
    if(NULL == handlePtr) return;
    XXudpSocketContext *context = *handlePtr;
    
    if (context->socket > 0) {
        close(context->socket);
    }
    for (int i = 0; i<5; i++) {
        free(context->addressArray[i]);
    }
    
    free(context);
    *handlePtr = NULL;
}

void msecToTimeVal(struct timeval *time, time_t msec){
    time->tv_usec   = msec % 1000 * 1000;
    time->tv_sec    = msec / 1000;
}
static ssize_t loopToSend(int socket, int8_t *data, ssize_t length, int times, struct timeval *timeval, fd_set *fdwrite, const char *ip, int port){
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(NULL == ip?"255.255.255.255":ip);
    
    for (int index = 0; index < times; index++) {
        FD_ZERO(fdwrite);
        FD_SET(socket, fdwrite);
        
        int ret = select(socket + 1, NULL, fdwrite, NULL, timeval);
        if(ret <= 0){
            continue;
        }
        else{
            break;
        }
    }
    
    ssize_t size = sendto(socket, data, length, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (length == size) {
        return (int)size;
    }
    return -1;
}
static ssize_t loopToRecv(int socket, int8_t *data, ssize_t maxLength, int times, struct timeval *timeval, fd_set *fdread, bool *hasSomething, int *recvCountAfterSelect){
    struct sockaddr addr;
    socklen_t addrBufferSize = sizeof(addr);
    bzero(&addr, addrBufferSize);
    
    ssize_t receivedSize = 0;
    //printf("[GWX] [XXudpSocket] [循环接收] 目标大小:%zd 循环次数:%d 单次时间:%ld\r\n", maxLength, times, timeval->tv_sec * 1000 + timeval->tv_usec / 1000);
    for (int index = 0; index < times; index++) {
        if (!*hasSomething) {
            FD_ZERO(fdread);
            FD_SET(socket, fdread);
            int ret = select(socket + 1, fdread, NULL, NULL, timeval);
            if (ret < 0) {
                //printf("[GWX] [XXudpSocket] [%d] socket error\n", __LINE__);
                return -100;
            }
            else if(0 == ret) {
                continue;
            }
            else {
                *hasSomething = true;
                *recvCountAfterSelect = 0;
                //printf("[GWX] [XXudpSocket] [%d] select通过\n", __LINE__);
            }
        }
        else{
            //printf("[GWX] [XXudpSocket] [%d] 之前有数据跳过select\n", __LINE__);
        }
        
//        ssize_t size = recv(socket, data + receivedSize, maxLength - receivedSize, MSG_DONTWAIT);
        ssize_t size = recvfrom(socket, data+receivedSize, maxLength-receivedSize, MSG_DONTWAIT, &addr, &addrBufferSize);
        if (size < 0) {
            //printf("[GWX] [XXudpSocket] [%d] socket error\n", __LINE__);
            if (*hasSomething) {
                *hasSomething = false;
                continue;
            }
            else{
               return -101;
            }
        }
        else if (0 == size) {
            if (0 == *recvCountAfterSelect) {
                return XXSOCKET_ERROR_CLOSED;
            }
            else{
                *recvCountAfterSelect   = 0;
                *hasSomething           = false;
                continue;
            }
        }
        else{
            *recvCountAfterSelect += 1;
        }
        
        receivedSize += size;
        if (receivedSize > maxLength) {
            //printf("[GWX] [XXudpSocket] [%d] socket error\n", __LINE__);
            return -102;
        }
        else if(receivedSize == maxLength) {
            //printf("[GWX] [XXudpSocket] 接收完毕 %d/%d\n", receivedSize, maxLength);
            break;
        }
        else {
            //printf("[GWX] [XXudpSocket] %d >> %d/%d\n", size, receivedSize, maxLength);
        }
        
        //struct timeval start, end;
        //gettimeofday( &start, NULL );
        //printf("[GWX] [XXudpSocket] [循环接收] sleep开始:%ld\r\n", start.tv_sec * 1000 + start.tv_usec / 1000);
        //usleep(20000);
        //gettimeofday( &end, NULL );
        //printf("[GWX] [XXudpSocket] [循环接收] sleep结束:%ld\r\n", end.tv_sec * 1000 + end.tv_usec / 1000);
    }
    
    return receivedSize;
}
