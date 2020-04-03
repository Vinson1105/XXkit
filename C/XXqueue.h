/**
 * 2020-04-03
 * 1、增加必要性注释
 *
 * 2019-05-27
 * 1、基于linux内核的双向内嵌链表（list.h）实现的本队列
 *
 * 2、关于void (*XXqueueDataCopyFunc)(void *dest, void *src, int length);
 *      dest 是已经分配好空间，需要调用者根据自定义的数据结构进行空间分配或值复制
 *      src  需要复制的源对象
 *      length dest和src的字节数
 *
 * 3、关于void (*XXqueueDataFreeFunc)(void *data, int length);
 *      data 需要释放的空间，但是不需要调用者 free(data) ,只需要对【dataCopyFunc】产生的数据负责
 *      length data的字节数
 *
 * 4、dequeue和enqueue中的length都是指示了队列中元素的数据长度，并不能指示元素中的指针指向的数据长度
 *      元素定义如下：
 *      struct item{
 *          uint32 id,
 *          uint8 *data,
 *      };
 *      length = sizeof(struct item);
 *      而data指向的数据长度并不包含在length中
 */

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
    
/**
 * @brief    创建队列句柄，dataCopyFunc和dataFreeFunc需要同步，不能一个为NULL，另一个不是NULL
 * @param    dataCopyFunc 队列中元素的复制回调，可以为NULL
 * @param    dataFreeFunc 队列中元素的释放回调，可以为NULL
 * @return   失败返回NULL
 *
 */
XXqueueHandle xxqueue_create(XXqueueDataCopyFunc dataCopyFunc, XXqueueDataFreeFunc dataFreeFunc);

/**
 * @brief    创建队列句柄
 * @param    dataCopyFunc 队列中元素的复制回调
 * @param    dataFreeFunc 队列中元素的释放回调
 * @param    maxCount 队列中最大容量
 * @param    maxHandle 达到最大容量是的操作，1：掉弃第一个元素，2：掉弃最后一个元素（2暂时没有实现）
 * @return   失败返回NULL
 */
XXqueueHandle xxqueue_createWithLimit(XXqueueDataCopyFunc dataCopyFunc, XXqueueDataFreeFunc dataFreeFunc, int maxCount, int maxHandle);   // maxHandle=1:丢头,=2:丢尾

/**
 * @brief    加入数据到队列，会执行copy，data的控制权还是在调用者手上，需要注意释放问题
 * @param    handle 队列句柄
 * @param    data 数据
 * @param    length
 * @return   成功返回0，否则加入失败，-1：数据有误，XXQUEUE_IS_FULL：队列已满
 */
int xxqueue_enqueue(XXqueueHandle handle, void *data, int length);

/**
 * @brief    从队列中取出数据，会执行copy和free；会将【元素】copy->【data】，free【元素】
 * @param    handle 队列句柄
 * @param    data 存储数据的控件，需要提前进行分配空间
 * @param    length 取出的数据长度
 * @return   成功返回0，否则加入失败，-1：数据有误，XXQUEUE_IS_FULL：队列已满
 */
int xxqueue_dequeue(XXqueueHandle handle, void *data, int *length);

/**
 * @brief    返回队列中的元素数量
 * @param    handle 队列句柄
 * @return   当前handle==NULL时，返回-1，否则返回实际队列元素数量
 */
int xxqueue_count(XXqueueHandle handle);

/**
 * @brief    删除队列中任意位置的元素，当前index有效时会触发free
 * @param    handle 队列句柄
 * @param    index 元素位置
 */
void xxqueue_removeOne(XXqueueHandle handle, int index);

/**
 * @brief    删除队列中所有元素，当队列不为空时会触发free
 * @param    handle 队列句柄
 */
void xxqueue_removeAll(XXqueueHandle handle);

/**
 * @brief    删除队列中所有元素，并释放句柄资源，当队列不为空时会触发free
 * @param    handlePtr 队列句柄的引用（注意：并不是handle，而是&handle）
 */
void xxqueue_free(XXqueueHandlePtr handlePtr);
    
#ifdef __cplusplus
}
#endif

#endif /* XXqueue_h */
