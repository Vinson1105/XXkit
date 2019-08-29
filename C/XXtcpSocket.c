#include "XXtcpSocket.h"
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

#include <errno.h>
//#define USING_ADDRESS_PTR

#define DECLARE_CONTEXT(handle) XXtcpSocketContext *context = (XXtcpSocketContext*)(handle);
typedef struct sockaddr     SocketAddress;
typedef struct sockaddr_in  SocketAddress4;
typedef struct sockaddr_in6 SocketAddress6;

typedef char* XXaddress;
typedef char** XXaddressArray;

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
    
    char    *addressArray[5];
}XXtcpSocketContext;

// 私有函数声明 {
static void msecToTimeVal(struct timeval *time, time_t msec);
static ssize_t loopToRecv(int socket, uint8_t *data, ssize_t maxLength, int times, struct timeval *timeval, fd_set *fdread, bool *hasSomething, int *recvCountAfterSelect);

static int lookupHost(const char *host, uint16_t port, char** addressArray, int *addressCapacity);
static bool isIPV6Address(char *address);
static bool hostFromAddress(char *hostBuffer, char *address);
static bool getHost(char *hostBuffer, uint16_t *port, sa_family_t *family, char *address);

static bool hostFromSockaddr4(char *hostBuffer, const struct sockaddr_in *sockaddr4);
static bool hostFromSockaddr6(char *hostBuffer, const struct sockaddr_in6 *sockaddr6);
static uint16_t portFromSockaddr4(const struct sockaddr_in *sockaddr4);
static uint16_t portFromSockaddr6(const struct sockaddr_in6 *sockaddr6);

static void printfHex(char *title, char *data, int length);
// } 私有函数声明

XXtcpSocketHandle xxtcpSocket_alloc(int recvBufferSize, int recvTryTimes, time_t waitSelectMsec){
    XXtcpSocketContext *context = (XXtcpSocketContext*)malloc(sizeof(XXtcpSocketContext));
    context->socket             = -1;
    context->recvTryTimes       = recvTryTimes;
    context->recvBufferSize     = recvBufferSize;
    context->isConnected        = false;
    msecToTimeVal(&(context->selectTime), waitSelectMsec);

    context->hasSomething       = false;
    context->recvCountAfterSelect = 0;
    
    for (int i = 0; i<5; i++) {
        char *buffer = malloc(128);
        memset(buffer, 0, 128);
        context->addressArray[i] = buffer;
    }
    return context;
}
int xxtcpSocket_connectToHost(XXtcpSocketHandle handle, const char *ip, int port){
    DECLARE_CONTEXT(handle)
    if (NULL == context || context->isConnected) {
        return -1;
    }

    // ipv6兼容
    char realHost[128] = {0};
    int addressCapacity = 0;
    int ret = lookupHost(ip, port, context->addressArray, &addressCapacity);
    if(0 != ret && 0 == addressCapacity){
        return -1;
    }
    bool isIPV6 = isIPV6Address(context->addressArray[0]);
    if (isIPV6) {
        hostFromAddress(realHost, context->addressArray[0]);
    }
    else{
        strcpy(realHost, ip);
    }
    printf("\t---> [XXtcpSocket] IP:%s Port:%d ---> %s(IPV%s)\n", ip, port, realHost, isIPV6?"6":"4");
    
    // 检测IP类型,并组成对应的addr结构体
    SocketAddress4 address4;    bzero(&address4, sizeof(SocketAddress4));
    SocketAddress6 address6;    bzero(&address6, sizeof(SocketAddress6));
    int addressLength   = 0;
    int newSocket       = 0;
    if(!isIPV6){
        addressLength               = sizeof(SocketAddress4);
        address4.sin_family         = AF_INET;
        address4.sin_port           = htons(port);
        address4.sin_addr.s_addr    = inet_addr(realHost);
        newSocket                   = socket(AF_INET, SOCK_STREAM, 0);
    }
    else{
        addressLength               = sizeof(SocketAddress6);
        address6.sin6_family        = AF_INET6;
        address6.sin6_port          = htons(port);
        inet_pton(AF_INET6, realHost, &(address6.sin6_addr));
        newSocket                   = socket(AF_INET6, SOCK_STREAM, 0);
    }
    if (newSocket <= 0) {
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
    if(0 != connect(newSocket, (isIPV6?(SocketAddress*)&address6:(SocketAddress*)&address4), addressLength)){
        //由于是非阻塞,connect并不会马上执行,所以此处不能就此判断是否连接成功
        //printf("\t---> [XXtcpSocket] [&] 连接失败 errno=%d\n", errno);
        //return -1;
    }

    context->socket         = newSocket;
    context->isConnected    = true;

    // 设置socket为检测点
    FD_ZERO(&(context->fdread));
    FD_SET(newSocket, &context->fdread);

    FD_ZERO(&(context->fdwrite));
    FD_SET(newSocket, &context->fdwrite);

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
    for (int i = 0; i<5; i++) {
        free(context->addressArray[i]);
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

static int lookupHost(const char *host, uint16_t port, char** addressArray, int *addressCapacity)
{
    *addressCapacity = 0;
    if (0 == strcmp(host, "localhost") || 0 == strcmp(host, "loopback")){
        // Use LOOPBACK address
        struct sockaddr_in nativeAddr4;
        nativeAddr4.sin_len         = sizeof(struct sockaddr_in);
        nativeAddr4.sin_family      = AF_INET;
        nativeAddr4.sin_port        = htons(port);
        nativeAddr4.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        memset(&(nativeAddr4.sin_zero), 0, sizeof(nativeAddr4.sin_zero));
        
        struct sockaddr_in6 nativeAddr6;
        nativeAddr6.sin6_len        = sizeof(struct sockaddr_in6);
        nativeAddr6.sin6_family     = AF_INET6;
        nativeAddr6.sin6_port       = htons(port);
        nativeAddr6.sin6_flowinfo   = 0;
        nativeAddr6.sin6_addr       = in6addr_loopback;
        nativeAddr6.sin6_scope_id   = 0;
        
        // Wrap the native address structures
        *addressCapacity = 2;
        memcpy(addressArray[0], &nativeAddr4, sizeof(nativeAddr4));
        memcpy(addressArray[1], &nativeAddr6, sizeof(nativeAddr6));
    }
    else
    {
        struct addrinfo hints, *res, *res0;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family   = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        
        char portString[16] = {0};
        sprintf(portString,"%hu",port);
        int gai_error = getaddrinfo(host, portString, &hints, &res0);
        if (gai_error)
        {
            printf("\t---> [XXtcpSocket] %s %d(%d %d) error:%s\n", host, port, *(const char*)&port, *((const char*)&port+1), gai_strerror(gai_error));
            return gai_error;
        }
        else
        {
            int capacity = 0;
            for (res = res0; res; res = res->ai_next)
            {
                if (res->ai_family == AF_INET || res->ai_family == AF_INET6) {
                    
                    if (res->ai_family == AF_INET)
                    {
                    }
                    else if (res->ai_family == AF_INET6) {
                    }
                    
                    capacity++;
                }
            }
            
            *addressCapacity = capacity;
            int index = 0;
            for (res = res0; res; res = res->ai_next, index++)
            {
                if (res->ai_family == AF_INET)
                {
                    // Found IPv4 address.
                    // Wrap the native address structure, and add to results.
                    memcpy(addressArray[index], res->ai_addr, res->ai_addrlen);
                }
                else if (res->ai_family == AF_INET6)
                {
                    // Fixes connection issues with IPv6
                    // https://github.com/robbiehanson/CocoaAsyncSocket/issues/429#issuecomment-222477158
                    
                    // Found IPv6 address.
                    // Wrap the native address structure, and add to results.
                    
                    struct sockaddr_in6 *sockaddr = (struct sockaddr_in6 *)res->ai_addr;
                    in_port_t *portPtr = &sockaddr->sin6_port;
                    if ((portPtr != NULL) && (*portPtr == 0)) {
                        *portPtr = htons(port);
                    }
                    memcpy(addressArray[index], res->ai_addr, res->ai_addrlen);
                }
            }
            freeaddrinfo(res0);
        }
    }
    return 0;
}
static bool isIPV6Address(char *address){
    //if (strlen(address) >= sizeof(struct sockaddr))
    //{
        const struct sockaddr *sockaddrX = (const struct sockaddr *)address;
        if (sockaddrX->sa_family == AF_INET6) {
            return true;
        }
    //}
    
    return false;
}
static bool hostFromAddress(char *hostBuffer, char *address)
{
    return getHost(hostBuffer, NULL, NULL, address);
}
static bool getHost(char *hostBuffer, uint16_t *port, sa_family_t *family, char *address)
{
    //if (strlen(address) >= sizeof(struct sockaddr))
    //{
        const struct sockaddr *sockaddrX = (const struct sockaddr *)address;
        
        if (sockaddrX->sa_family == AF_INET)
        {
            //if (strlen(address) >= sizeof(struct sockaddr_in))
            //{
                struct sockaddr_in sockaddr4;
                memcpy(&sockaddr4, sockaddrX, sizeof(sockaddr4));
                
                if (hostBuffer) hostFromSockaddr4(hostBuffer, &sockaddr4);
                if (port)       *port = portFromSockaddr4(&sockaddr4);
                if (family)     *family = AF_INET;
                return true;
            //}
        }
        else if (sockaddrX->sa_family == AF_INET6)
        {
            //if (strlen(address) >= sizeof(struct sockaddr_in6))
            //{
                struct sockaddr_in6 sockaddr6;
                memcpy(&sockaddr6, sockaddrX, sizeof(sockaddr6));
                
                if (hostBuffer) hostFromSockaddr6(hostBuffer, &sockaddr6);
                if (port)       *port = portFromSockaddr6(&sockaddr6);
                if (family)     *family = AF_INET6;
                return true;
            //}
        }
    //}
    
    return false;
}
static bool hostFromSockaddr4(char *hostBuffer, const struct sockaddr_in *sockaddr4)
{
    if (inet_ntop(AF_INET, &sockaddr4->sin_addr, hostBuffer, INET_ADDRSTRLEN) == NULL)
    {
        return false;
    }
    return true;
}

static bool hostFromSockaddr6(char *hostBuffer, const struct sockaddr_in6 *sockaddr6)
{
    if (inet_ntop(AF_INET6, &sockaddr6->sin6_addr, hostBuffer, INET6_ADDRSTRLEN) == NULL)
    {
        return false;
    }
    return true;
}

static uint16_t portFromSockaddr4(const struct sockaddr_in *sockaddr4)
{
    return ntohs(sockaddr4->sin_port);
}

static uint16_t portFromSockaddr6(const struct sockaddr_in6 *sockaddr6)
{
    return ntohs(sockaddr6->sin6_port);
}

static void printfHex(char *title, char *data, int length){
    printf("%s", title);
    for (int index = 0; index < length; index++) {
        printf("%02x ", (uint8_t)*(data+index) );
    }
    printf("\n");
}
