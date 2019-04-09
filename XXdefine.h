/*
 * @author: GWX
 * @date: 20190109
 * @descirption: XX库全局定义
 * @history:
 *        1.author:
 *          date:
 *          modification:
 */

#ifndef __XXdefine.h
#define __XXdefine.h

#include <stdint.h>

typedef struct XXbuffer{
    uint8_t *data;
    uint64_t length;
}XXbuffer;

#define XX_DEBUG    // 调试信息输出标记

#endif
