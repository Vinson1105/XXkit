#include "XXtcpSocket.h"
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

#define DECLARE_CONTEXT(handle) XXtcpSocketContext *context = (XXtcpSocketContext*)(handle);

typedef struct XXtcpSocketContext{
    fd_set  fdwrite;
    fd_set  fdread;
    int     socket;
    int     recvTryTimes;
    int     recvBufferSize;
    struct  timeval selectTime;
    bool    isConnected;
    
    bool    hasSomething;
    int     recvCountAfterSelect;
}XXtcpSocketContext;

// 私有函数声明 {
static void msecToTimeVal(struct timeval *time, time_t msec);
static ssize_t loopToRecv(int socket, uint8_t *data, ssize_t maxLength, int times, struct timeval *timeval, fd_set *fdread, bool *hasSomething, int *recvCountAfterSelect);
// } 私有函数声明

XXtcpSocketHandle xxtcpSocket_alloc(int recvBufferSize, int recvTryTimes, time_t waitSelectMsec){
    XXtcpSocketContext *context = (XXtcpSocketContext*)malloc(sizeof(XXtcpSocketContext));
    context->socket             = -1;
    context->recvTryTimes       = recvTryTimes;
    context->recvBufferSize     = recvBufferSize;
    context->isConnected        = false;
    context->hasSomething       = false;
    context->recvCountAfterSelect = 0;
    msecToTimeVal(&(context->selectTime), waitSelectMsec);
    return context;
}
int xxtcpSocket_connectToHost(XXtcpSocketHandle handle, const char *ip, int port){
    DECLARE_CONTEXT(handle)
    if (NULL == context || context->isConnected) {
        return -1;
    }

    // 检测IP类型,并组成对应的addr结构体
    struct sockaddr *sockaddr;
    int sockaddrLength;
    int newSocket = 0;
    if(xxnet_isIPV4(ip)){
        sockaddrLength              = sizeof(struct sockaddr_in);
        struct sockaddr_in *addr    = (struct sockaddr_in*)malloc(sockaddrLength);
        addr->sin_family            = AF_INET;
        addr->sin_port              = htons(port);
        addr->sin_addr.s_addr       = inet_addr(ip);
        
        sockaddr    = (struct sockaddr *)addr;
        newSocket   = socket(AF_INET, SOCK_STREAM, 0);
        if (newSocket < 0) {
            return -1;
        }
    }
    else if (xxnet_isIPV6(ip)) {
        sockaddrLength              = sizeof(struct sockaddr_in6);
        struct sockaddr_in6 *addr   = (struct sockaddr_in6*)malloc(sockaddrLength);
        addr->sin6_family           = AF_INET6;
        addr->sin6_port             = htons(port);
        inet_pton(AF_INET6, ip, &addr->sin6_addr);

        sockaddr    = (struct sockaddr *)addr;
        newSocket   = socket(AF_INET6, SOCK_STREAM, 0);
        if (newSocket < 0) {
            return -1;
        }
    }
    else{
        return -1;
    }

    // 关闭Nagle和所在线程已关闭报错
    int on = 1;
    setsockopt(newSocket, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));   // 关闭Nagle算法,不等待未确认包的ack
    setsockopt(newSocket, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof(on));   // 避免线程已经关闭,但是线程中的socket没有关闭导致的错误
    setsockopt(newSocket, SOL_SOCKET, SO_RCVBUF, &(context->recvBufferSize), sizeof(context->recvBufferSize));  // 设置接收buffer的大小
    
    // 开启非阻塞
    uint flags = fcntl(newSocket, F_GETFL, 0);
    fcntl(newSocket, F_SETFL, flags|O_NONBLOCK);
    
    // 连接
    if(0 != connect(newSocket, sockaddr, sockaddrLength)){
        //由于是非阻塞,connect并不会马上执行,所以此处不能就此判断是否连接成功
        //free(sockaddr);
        //return -1;
    }

    context->socket         = newSocket;
    context->isConnected    = true;

    // 设置socket为检测点
    FD_ZERO(&(context->fdread));
    FD_SET(newSocket, &context->fdread);

    FD_ZERO(&(context->fdwrite));
    FD_SET(newSocket, &context->fdwrite);

    free(sockaddr);
    return 0;
}
ssize_t xxtcpSocket_send(XXtcpSocketHandle handle, uint8_t *data, ssize_t length){
    DECLARE_CONTEXT(handle)
    if (NULL == context || context->socket < 0 || !context->isConnected) {
        return -1;
    }
    
    FD_ZERO(&(context->fdwrite));
    FD_SET(context->socket, &context->fdwrite);
    
    if (select(context->socket + 1, NULL, &(context->fdwrite), NULL, &(context->selectTime)) <= 0){
        return -1;
    }
    
    return send(context->socket, data, length, 0);
}
ssize_t xxtcpSocket_recv(XXtcpSocketHandle handle, uint8_t *data, ssize_t length){
    DECLARE_CONTEXT(handle)
    if (NULL == context || context->socket < 0 || !context->isConnected) {
        printf("[GWX] error/n");
        return -1;
    }
    memset(data, 0, length);
    return loopToRecv(context->socket, data, length, context->recvTryTimes, &(context->selectTime), &(context->fdread), &(context->hasSomething), &(context->recvCountAfterSelect));
}
ssize_t xxtcpSocket_waitForRecv(XXtcpSocketHandle handle, uint8_t *data, ssize_t maxLength, int times, time_t waitSelectMsec){
    DECLARE_CONTEXT(handle)
    if (NULL == context || context->socket < 0 || !context->isConnected) {
        return -1;
    }
    
    memset(data, 0, maxLength);
    struct timeval selectTime;
    msecToTimeVal(&selectTime, waitSelectMsec);
    return loopToRecv(context->socket, data, maxLength, context->recvTryTimes, &selectTime, &(context->fdread), &(context->hasSomething), &(context->recvCountAfterSelect));
}
void xxtcpSocket_disconnect(XXtcpSocketHandle handle){
    DECLARE_CONTEXT(handle)
    if (NULL == context || context->socket < 0 || !context->isConnected) {
        return;
    }
    
    close(context->socket);
    context->isConnected = false;
}
void xxtcpSocket_free(XXtcpSocketHandle *handle){
    DECLARE_CONTEXT(*handle)
    if (NULL == context) {
        return;
    }
    
    if (context->socket > 0 && context->isConnected) {
        close(context->socket);
    }
    
    free(context);
    *handle = NULL;
}
int xxtcpSocket_socket(XXtcpSocketHandle handle){
    DECLARE_CONTEXT(handle)
    return context->socket;
}

//
//
//
static void msecToTimeVal(struct timeval *time, time_t msec){
    time->tv_usec   = msec % 1000 * 1000;
    time->tv_sec    = msec / 1000;
}
static ssize_t loopToRecv(int socket,
                          uint8_t *data,
                          ssize_t maxLength,
                          int times,
                          struct timeval *timeval,
                          fd_set *fdread,
                          bool *hasSomething,
                          int *recvCountAfterSelect){
    ssize_t receivedSize = 0;
    //printf("[GWX] [XXtcpSocket] [循环接收] 目标大小:%zd 循环次数:%d 单次时间:%ld\r\n", maxLength, times, timeval->tv_sec * 1000 + timeval->tv_usec / 1000);
    for (int index = 0; index < times; index++) {
        if (!*hasSomething) {
            FD_ZERO(fdread);
            FD_SET(socket, fdread);
            int ret = select(socket + 1, fdread, NULL, NULL, timeval);
            if (ret < 0) {
                //printf("[GWX] [XXtcpSocket] [%d] socket error\n", __LINE__);
                return -100;
            }
            else if(0 == ret) {
                continue;
            }
            else {
                *hasSomething = true;
                *recvCountAfterSelect = 0;
                //printf("[GWX] [XXtcpSocket] [%d] select通过\n", __LINE__);
            }
        }
        else{
            //printf("[GWX] [XXtcpSocket] [%d] 之前有数据跳过select\n", __LINE__);
        }
        
        ssize_t size = recv(socket, data + receivedSize, maxLength - receivedSize, MSG_DONTWAIT);
        if (size < 0) {
            //printf("[GWX] [XXtcpSocket] [%d] socket error\n", __LINE__);
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
            //printf("[GWX] [XXtcpSocket] [%d] socket error\n", __LINE__);
            return -102;
        }
        else if(receivedSize == maxLength) {
            //printf("[GWX] [XXtcpSocket] 接收完毕 %d/%d\n", receivedSize, maxLength);
            break;
        }
        else {
            //printf("[GWX] [XXtcpSocket] %d >> %d/%d\n", size, receivedSize, maxLength);
        }
        
        //struct timeval start, end;
        //gettimeofday( &start, NULL );
        //printf("[GWX] [XXtcpSocket] [循环接收] sleep开始:%ld\r\n", start.tv_sec * 1000 + start.tv_usec / 1000);
        //usleep(20000);
        //gettimeofday( &end, NULL );
        //printf("[GWX] [XXtcpSocket] [循环接收] sleep结束:%ld\r\n", end.tv_sec * 1000 + end.tv_usec / 1000);
    }
    
    return receivedSize;
}

