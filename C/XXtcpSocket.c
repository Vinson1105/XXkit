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

#define DECLARE_CONTEXT(handle) XXtcpSocketContext *context = (XXtcpSocketContext*)(handle);

typedef struct XXtcpSocketContext{
    fd_set  fdwrite;
    fd_set  fdread;
    int     socket;
    int     recvTryTimes;
    int     recvBufferSize;
    struct  timeval selectTime;
    bool    isConnected;
}XXtcpSocketContext;

// 私有函数声明 {
static void msecToTimeVal(struct timeval *time, time_t msec);
static ssize_t loopToRecv(int socket, uint8_t *data, ssize_t maxLength, int times, struct timeval *timeval, fd_set *fdread);
// } 私有函数声明

XXtcpSocketHandle xxtcpSocket_alloc(int recvBufferSize, int recvTryTimes, time_t waitSelectMsec){
    XXtcpSocketContext *context = (XXtcpSocketContext*)malloc(sizeof(XXtcpSocketContext));
    context->socket             = -1;
    context->recvTryTimes       = recvTryTimes;
    context->recvBufferSize     = recvBufferSize;
    context->isConnected        = false;
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
    }
    else if (xxnet_isIPV6(ip)) {
        sockaddrLength              = sizeof(struct sockaddr_in6);
        struct sockaddr_in6 *addr   = (struct sockaddr_in6*)malloc(sockaddrLength);
        addr->sin6_family           = AF_INET6;
        addr->sin6_port             = htons(port);
        inet_pton(AF_INET6, ip, &addr->sin6_addr);

        sockaddr    = (struct sockaddr *)addr;
        newSocket   = socket(AF_INET6, SOCK_STREAM, 0);
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
    if (NULL == context || !context->isConnected) {
        return -1;
    }
    
    FD_ZERO(&(context->fdwrite));
    FD_SET(context->socket, &context->fdwrite);
    
    if (select(context->socket + 1,NULL, &(context->fdwrite), NULL, &(context->selectTime)) <= 0){
        return -1;
    }
    
    return send(context->socket, data, length, 0);
}
ssize_t xxtcpSocket_recv(XXtcpSocketHandle handle, uint8_t *data, ssize_t length){
    DECLARE_CONTEXT(handle)
    if (NULL == context || !context->isConnected) {
        return -1;
    }
    
    return loopToRecv(context->socket, data, length, context->recvTryTimes, &(context->selectTime), &(context->fdread));
}
ssize_t xxtcpSocket_waitForRecv(XXtcpSocketHandle handle, uint8_t *data, ssize_t maxLength, int times, time_t msec){
    DECLARE_CONTEXT(handle)
    if (NULL == context || !context->isConnected) {
        return -1;
    }
    
    struct timeval selectTime;
    msecToTimeVal(&selectTime, msec);
    return loopToRecv(context->socket, data, maxLength, context->recvTryTimes, &selectTime, &(context->fdread));
}
void xxtcpSocket_disconnect(XXtcpSocketHandle handle){
    DECLARE_CONTEXT(handle)
    if (NULL == context || !context->isConnected) {
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
    
    if (context->isConnected) {
        close(context->socket);
    }
    
    free(context);
    *handle = NULL;
}

//
//
//
static void msecToTimeVal(struct timeval *time, time_t msec){
    time->tv_usec   = msec % 1000 * 1000;
    time->tv_sec    = msec / 1000;
}
static ssize_t loopToRecv(int socket, uint8_t *data, ssize_t maxLength, int times, struct timeval *timeval, fd_set *fdread){
    ssize_t receivedSize = 0;
    //printf("[GWX] [XXtcpSocket] [循环接收] 目标大小:%zd 循环次数:%d 单次时间:%ld\r\n", maxLength, times, timeval->tv_sec * 1000 + timeval->tv_usec / 1000);
    for (int index = 0; index < times; index++) {
        FD_ZERO(fdread);
        FD_SET(socket, fdread);
        int ret = select(socket + 1, fdread, NULL, NULL, timeval);
        
        if (ret < 0) {
            return -1;
        }
        else if(0 == ret) {
            //usleep(20000);
            //printf("[GWX] [XXtcpSocket] [循环接收] select超时\r\n");
            continue;
        }
        else {
            //printf("[GWX] [XXtcpSocket] [循环接收] select通过\r\n");
        }
        
        ssize_t size = recv(socket, data + receivedSize, maxLength - receivedSize, 0);
        if (size <= 0) {
            return -1;
        }
        
        receivedSize += size;
        if (receivedSize > maxLength) {
            return -1;
        }
        else if(receivedSize == maxLength) {
            break;
        }
        else {
            //printf("[GWX] [XXtcpSocket] [循环接收] 收到:%zd 总接收:%zdd\r\n", size, receivedSize);
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

