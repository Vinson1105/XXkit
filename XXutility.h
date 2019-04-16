/*
 * @author: GWX
 * @date: 20190412
 * @descirption: XX库通用模块
 * @history:
 *        1.author:
 *          date:
 *          modification:
 */

#ifndef XXutility_h
#define XXutility_h

#include <stdint.h>
#include <stdbool.h>

typedef struct XXdata{
    int8_t *data;
    uint64_t length;
}XXdata;

typedef struct XXbuffer{
    int8_t *data;
    uint64_t currentLength;
    uint64_t maxLength;
}XXbuffer;

#endif
