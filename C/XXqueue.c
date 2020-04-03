#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "XXqueue.h"
#include "list.h"

typedef struct XXqueueItem{
    void *data;
    int length;
    struct list_head list;
}XXqueueItem;

typedef struct XXqueueContext{
    XXqueueItem headItem;               // 头部item
    XXqueueDataCopyFunc dataCopyFunc;   // item复制函数
    XXqueueDataFreeFunc dataFreeFunc;   // item释放函数
    int maxCount;                       // 最大数量
    int maxHandle;
    int count;
}XXqueueContext;

static void freeQueueItem(XXqueueContext *context, XXqueueItem *item);
static XXqueueItem* posQueueItem(XXqueueContext *context, int index);

XXqueueHandle xxqueue_create(XXqueueDataCopyFunc dataCopyFunc, XXqueueDataFreeFunc dataFreeFunc){
    XXqueueContext *context = (XXqueueContext*)malloc(sizeof(XXqueueContext));
    context->maxCount       = 0;
    context->count          = 0;
    context->dataCopyFunc   = dataCopyFunc;
    context->dataFreeFunc   = dataFreeFunc;
    
    INIT_LIST_HEAD(&(context->headItem.list));
    return context;
}
XXqueueHandle xxqueue_createWithLimit(XXqueueDataCopyFunc dataCopyFunc, XXqueueDataFreeFunc dataFreeFunc, int maxCount, int maxHandle){
    XXqueueContext *context = (XXqueueContext*)malloc(sizeof(XXqueueContext));
    context->maxCount       = maxCount;
    context->maxHandle      = maxHandle;
    context->count          = 0;
    context->dataCopyFunc   = dataCopyFunc;
    context->dataFreeFunc   = dataFreeFunc;
    
    INIT_LIST_HEAD(&(context->headItem.list));
    return context;
}

int xxqueue_enqueue(XXqueueHandle handle, void *data, int length){
    XXqueueContext *context = (XXqueueContext*)handle;
    if (NULL == context || NULL == data || 0 == length) {
        return -1;
    }
    
    if (context->maxCount > 0 && context->count >= context->maxCount) {
        if (1 == context->maxHandle) {
            xxqueue_removeOne(handle, 0);
        }
        else if (2 == context->maxHandle){
            // FIXME: 2是删除最有一个
            return XXQUEUE_IS_FULL;
        }
        else{
            return XXQUEUE_IS_FULL;
        }
    }
    XXqueueItem *newItem = (XXqueueItem*)malloc(sizeof(XXqueueItem));
    newItem->length = length;
    
    newItem->data = malloc(length);
    memset(newItem->data, 0, length);
    if (NULL != context->dataCopyFunc) {
         context->dataCopyFunc(newItem->data, data, length);
    }
    else{
        memcpy(newItem->data, (uint8_t*)data, length);
    }
    list_add_tail(&(newItem->list), &(context->headItem.list));
    
    context->count++;
    return 0;
}
int xxqueue_dequeue(XXqueueHandle handle, void *data, int *length){
    XXqueueContext *context = (XXqueueContext*)handle;
    if (NULL == context || NULL == data || NULL == length) {
        return -1;
    }
    
    if(list_empty(&context->headItem.list)){
        return XXQUEUE_IS_EMPTY;
    }
    
    struct list_head *pos = context->headItem.list.next;
    XXqueueItem *item = list_entry(pos, XXqueueItem, list);
    list_del(pos);
    
    if (NULL != context->dataCopyFunc) {
        context->dataCopyFunc(data, item->data, item->length);
    }
    else{
        memcpy(data, item->data, item->length);
    }
    *length = item->length;
    context->count--;
    freeQueueItem(context, item);
    
//    memcpy(data, item->data, item->length);
//    *length = item->length;
//    free(item);
//    context->count--;
    return 0;
}
int xxqueue_count(XXqueueHandle handle){
    XXqueueContext *context = (XXqueueContext*)handle;
    if (NULL == context) {
        return -1;
    }
    
    return context->count;
}
void xxqueue_removeOne(XXqueueHandle handle, int index){
    XXqueueContext *context = (XXqueueContext*)handle;
    if (NULL == context) {
        return;
    }
    
    XXqueueItem *item = posQueueItem(context, index);
    if (NULL == item) {
        return;
    }
    list_del(&(item->list));
    freeQueueItem(context, item);
    context->count--;
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
        list_del(pos);
        
        freeQueueItem(context, item);
    }
    context->count = 0;
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

void freeQueueItem(XXqueueContext *context, XXqueueItem *item){
    if (context->dataFreeFunc) {
        context->dataFreeFunc(item->data, item->length);
    }
    else{
        free(item->data);
    }
    free(item);
}
XXqueueItem* posQueueItem(XXqueueContext *context, int index){
    if(list_empty(&context->headItem.list)){
        return NULL;
    }
    
    struct list_head *pos = context->headItem.list.next;
    for (int i = 0; i < index; i++) {
        if (pos == &(context->headItem.list)) {
            return NULL;
        }
        pos = pos->next;
    }
    return list_entry(pos, XXqueueItem, list);
}
