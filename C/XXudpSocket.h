#ifndef XXudpSocket_h
#define XXudpSocket_h

#include <stdio.h>
#include <stdbool.h>

typedef void* XXudpSocketHandle;

#ifdef __cplusplus
extern "C"{
#endif
    
    XXudpSocketHandle xxudpSocket_alloc(int recvBufferSize, int recvTryTimes, time_t waitSelectMsec);
    int xxudpSocket_bind(XXudpSocketHandle handle, const char *host, int port, bool reusdPort);
    ssize_t xxudpSocket_sendto(XXudpSocketHandle handle, const char *host, int port, uint8_t *data, ssize_t length);
    ssize_t xxudpSocket_recvFrom(XXudpSocketHandle handle, uint8_t *data, ssize_t length, char *srcHost, int *srcPort, bool *isIPv6Src);
    void xxudpSocket_free(XXudpSocketHandle **handlePtr);
    
#ifdef __cplusplus
}
#endif

#endif /* XXudpSocket_h */
