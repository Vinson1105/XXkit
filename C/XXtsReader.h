/*
 * @author: GWX
 * @date: 20181229
 * @descirption: TS数据读取器,在TS包中获取帧数据
 * @history:
 */

#ifndef XXtsReader_h
#define XXtsReader_h

#include <stdbool.h>
#include <stdint.h>
#include "XXavDefine.h"

typedef void* XXtsHandle;
typedef void(*XXtsReaderLogCallBack)(const char *log, int length);
typedef void(*XXtsReaderFrameCallBack)(void *opaque, XXavFrame *frame);

#ifdef __cplusplus
extern "C"{
#endif    // __cplusplus

    /*
     * @brief   创建ts流拆解句柄
     * @param   opaque 作为回调函数'frameCallBack'的参数
     * @param   frameCallBack 获取到一帧数据时调用的回调
     * @param   logCallBack 日志输出回调,可以为NULL
     * @return  NULL:失败,否则成功
     */
    XXtsHandle xxtsReader_create(void *opaque, XXtsReaderFrameCallBack frameCallBack, XXtsReaderLogCallBack logCallBack);
    
    /*
     * @brief   添加待解析数据,注意如果待解析的数据包含多帧数据,此函数可能会重复调用'frameCallBack'
     * @param   handle 句柄
     * @param   data 待解析数据
     * @param   length 待解析数据长度
     */
    void xxtsReader_push(XXtsHandle handle, const char *data, int length);
    
    /*
     * @brief   释放句柄对应的数据,并把句柄置为NULL
     * @param   *handle 句柄地址
     */
    void xxtsReader_free(XXtsHandle *handle);

#ifdef __cplusplus
}
#endif    // __cplusplus

#endif // XXtsReader_h
