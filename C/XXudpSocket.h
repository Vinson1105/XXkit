//
//  XXudpSocket.h
//  teset
//
//  Created by VINSON on 2020/11/12.
//  Copyright © 2020 XX. All rights reserved.
//

#ifndef XXudpSocket_h
#define XXudpSocket_h

#include <stdio.h>
#include <stdbool.h>

#define XXSOCKET_ERROR_CLOSED       -3
typedef void* XXudpSocketHandle;

#ifdef __cplusplus
extern "C"{
#endif

XXudpSocketHandle xxudpSocket_alloc(int recvBufferSize, int tryTimes, time_t waitSelectMsec);
int xxudpSocket_bind(XXudpSocketHandle handle, int port);
int xxupdSocket_unbind(XXudpSocketHandle handle);
ssize_t xxudpSocket_send(XXudpSocketHandle handle, const char *ip, int port, int8_t *data, ssize_t length);
ssize_t xxudpSocket_recv(XXudpSocketHandle handle, int8_t *data, ssize_t length);
void xxudpSocket_free(XXudpSocketHandle *handlePtr);

#ifdef __cplusplus
}
#endif

#endif /* XXudpSocket_h */
