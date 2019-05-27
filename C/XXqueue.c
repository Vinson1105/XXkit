//
//  XXqueue.c
//  XXapp
//
//  Created by VINSON on 2019/5/27.
//  Copyright © 2019 VINSON. All rights reserved.
//

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "XXqueue.h"
#include "list.h"

typedef struct XXqueueItem{
    void *data;
    uint64_t length;
    struct list_head list;
}XXqueueItem;

typedef struct XXqueueContext{
    XXqueueItem headItem;               // 头部item
    XXqueueDataFreeFunc dataFreeFunc;   // item释放函数
    uint maxCount;                      // 最大数量
    uint count;
}XXqueueContext;

XXqueueHandle xxqueue_create(int maxCount, XXqueueDataFreeFunc dataFreeFunc){  
    XXqueueContext *context = (XXqueueContext*)malloc(sizeof(XXqueueContext));
    context->maxCount = maxCount;
    context->count = 0;
    context->dataFreeFunc = dataFreeFunc;
    
    INIT_LIST_HEAD(&(context->headItem.list));
    return context;
}
int xxqueue_enqueue(XXqueueHandle handle, void *data, uint64_t length){
    XXqueueContext *context = (XXqueueContext*)handle;
    if (NULL == context || NULL == data || 0 == length) {
        return -1;
    }
    
    if (context->maxCount > 0 && context->count >= context->maxCount) {
        return XXQUEUE_IS_FULL;
    }
    XXqueueItem *newItem = (XXqueueItem*)malloc(sizeof(XXqueueItem));
    newItem->length = length;
    newItem->data = malloc(length);
    memcpy(newItem->data, (uint8_t*)data, length);
    list_add(&(newItem->list), &(context->headItem.list));
    
    context->count++;
    return 0;
}
int xxqueue_dequeue(XXqueueHandle handle, void *data, uint64_t *length){
    XXqueueContext *context = (XXqueueContext*)handle;
    if (NULL == context || NULL == data || NULL == length) {
        return -1;
    }
    
    if(list_empty(&context->headItem.list)){
        return XXQUEUE_IS_EMPTY;
    }
    
    struct list_head *pos = context->headItem.list.next;
    XXqueueItem *item = list_entry(pos, XXqueueItem, list);
    list_del_init(pos);
    
    memcpy(data, item->data, item->length);
    *length = item->length;
    context->count--;
    free(item);
    return 0;
}
int xxqueue_count(XXqueueHandle handle){
    XXqueueContext *context = (XXqueueContext*)handle;
    if (NULL == context) {
        return -1;
    }
    
    return context->count;
}
void xxqueue_removeAll(XXqueueHandle handle){
    XXqueueContext *context = (XXqueueContext*)handle;
    if (NULL == context) {
        return;
    }
    
    struct list_head *pos, *next;
    XXqueueItem *item;
    list_for_each_safe(pos, next, &(context->headItem.list)){
        item = list_entry(pos, XXqueueItem, list);
        list_del_init(pos);
        
        if (context->dataFreeFunc) {
            context->dataFreeFunc(item->data, item->length);
        }
        else{
            free(item->data);
        }
        free(item);
    }
}
void xxqueue_free(XXqueueHandlePtr handlePtr){
    XXqueueContext **context = (XXqueueContext**)handlePtr;
    if (NULL == context || NULL == *context) {
        return;
    }
    
    xxqueue_removeAll(*context);
    free(*context);
    *context = NULL;
}
