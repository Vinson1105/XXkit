#include "XXnet.h"
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

#include <unistd.h>
#include <arpa/inet.h>

#define ADDRESS_BUFFER_ARRAY_SIZE   5
#define ADDRESS_BUFFER_SIZE         128

typedef struct sockaddr         SocketAddress;
typedef struct sockaddr_in      SocketAddress4;
typedef struct sockaddr_in6     SocketAddress6;

static bool getHost(char *host, uint16_t *port, sa_family_t *family, void *address);
static int lookupHost(const char *host, uint16_t port, void** addressArray, int *addressCapacity);

bool xxnet_transformHost(char *dest, bool *isIPv6, char *src, int port){
    bool isSucceed = true;
    // [1] 分配socketaddr结构体的存放空间
    char *addressArray[ADDRESS_BUFFER_ARRAY_SIZE] = {NULL};
    for (int i = 0; i < ADDRESS_BUFFER_ARRAY_SIZE; i++) {
        addressArray[i] = (char*)malloc(ADDRESS_BUFFER_SIZE);
        bzero(addressArray[i], ADDRESS_BUFFER_SIZE);
    }
    int addressCapacity = 0;
    
    // [2] 寻找对应的目标地址
    int ret = lookupHost(src, port, (void**)addressArray, &addressCapacity);
    if(0 != ret && 0 == addressCapacity){
        isSucceed = false;
        goto End;
    }
    
    // [3] 获取对应环境的ip(String)
    bool isIPV6 = xxnet_isIPv6((SocketAddress*)addressArray[0]);
    if (isIPV6) {
        xxnet_hostFromAddress6(dest, (SocketAddress6*)addressArray[0]);
    }
    else{
        strcpy(dest, src);
    }
    
End:
    // [4] 释放空间
    for (int i = 0; i < ADDRESS_BUFFER_ARRAY_SIZE; i++) {
        free(addressArray[i]);
    }
    return isSucceed;
}
bool xxnet_isIPv6(struct sockaddr *address){
    return AF_INET6 == address->sa_family;
}

bool xxnet_hostFromAddress4(char *host, struct sockaddr_in *address){
    return NULL != inet_ntop(AF_INET, &address->sin_addr, host, INET_ADDRSTRLEN);
}
int xxnet_portFromAddress4(struct sockaddr_in *address){
    return ntohs(address->sin_port);
}

bool xxnet_hostFromAddress6(char *host, struct sockaddr_in6 *address){
    return NULL != inet_ntop(AF_INET6, &address->sin6_addr, host, INET6_ADDRSTRLEN);
}
int xxnet_portFromAddress6(struct sockaddr_in6 *address){
    return ntohs(address->sin6_port);
}

void xxnet_infoFromAddress(char *host, int *port, bool *isIPv6, struct sockaddr *address){
    if (xxnet_isIPv6(address)) {
        if (host) xxnet_hostFromAddress6(host, (SocketAddress6*)address);
        if (port) *port = xxnet_portFromAddress6((SocketAddress6*)address);
        if (isIPv6) *isIPv6 = true;
    }
    else{
        if (host) xxnet_hostFromAddress4(host, (SocketAddress4*)address);
        if (port) *port = xxnet_portFromAddress4((SocketAddress4*)address);
        if (isIPv6) *isIPv6 = false;
    }
}

void xxnet_address4(SocketAddress4 *address, const char *host, int port){
    bzero(address, sizeof(SocketAddress4));
    address->sin_family         = AF_INET;
    address->sin_port           = htons(port);
    address->sin_addr.s_addr    = inet_addr(host);
}
void xxnet_address6(SocketAddress6 *address, const char *host, int port){
    bzero(address, sizeof(SocketAddress6));
    address->sin6_family = AF_INET6;
    address->sin6_port = htons(port);
    inet_pton(AF_INET6, host, &address->sin6_addr);
}


static int lookupHost(const char *host, uint16_t port, void** addressArray, int *addressCapacity){
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
            printf("[XXnet] %s %d(%d %d) error:%s\n", host, port, *(const char*)&port, *((const char*)&port+1), gai_strerror(gai_error));
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
static bool getHost(char *host, uint16_t *port, sa_family_t *family, void *address){
    const struct sockaddr *addressX = (const struct sockaddr *)address;
    if (addressX->sa_family == AF_INET){
        struct sockaddr_in address4;
        memcpy(&address4, addressX, sizeof(struct sockaddr_in));
        
        if (host)       xxnet_hostFromAddress4(host, &address4);
        if (port)       *port = xxnet_portFromAddress4(&address4);
        if (family)     *family = AF_INET;
        return true;
    }
    else if (addressX->sa_family == AF_INET6){
        struct sockaddr_in6 sockaddr6;
        memcpy(&sockaddr6, addressX, sizeof(sockaddr6));
        
        if (host)       xxnet_hostFromAddress6(host, &sockaddr6);
        if (port)       *port = xxnet_portFromAddress6(&sockaddr6);
        if (family)     *family = AF_INET6;
        return true;
    }
    else{
        
    }
    return false;
}
