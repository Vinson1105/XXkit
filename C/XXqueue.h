//
//  XXqueue.h
//  XXapp
//
//  Created by VINSON on 2019/5/27.
//  Copyright © 2019 VINSON. All rights reserved.
//

#ifndef XXqueue_h
#define XXqueue_h

#include <stdio.h>

#define XXQUEUE_IS_FULL     -10
#define XXQUEUE_IS_EMPTY    -11
typedef void* XXqueueHandle;
typedef void** XXqueueHandlePtr;
typedef void (*XXqueueDataCopyFunc)(void *dest, void *src, int length);
typedef void (*XXqueueDataFreeFunc)(void *data, int length);

#ifdef __cplusplus
extern "C" {
#endif
    
    XXqueueHandle xxqueue_create(XXqueueDataCopyFunc dataCopyFunc, XXqueueDataFreeFunc dataFreeFunc);
    XXqueueHandle xxqueue_createWithLimit(XXqueueDataCopyFunc dataCopyFunc, XXqueueDataFreeFunc dataFreeFunc, int maxCount, int maxHandle);   // maxHandle=1:丢头,=2:丢尾
    
    int xxqueue_enqueue(XXqueueHandle handle, void *data, int length);
    int xxqueue_dequeue(XXqueueHandle handle, void *data, int *length);
    int xxqueue_count(XXqueueHandle handle);
    void xxqueue_removeOne(XXqueueHandle handle, int index);
    void xxqueue_removeAll(XXqueueHandle handle);
    void xxqueue_free(XXqueueHandlePtr handlePtr);
    
#ifdef __cplusplus
}
#endif

#endif /* XXqueue_h */
