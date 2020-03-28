/**
 * XXnaluSpliter，基于【0x00 00 00 01】的nalu拆分器，
 *
 * 2020-03-28
 * 1、添加必要性注释
 */

#ifndef XXnaluSpliter_h
#define XXnaluSpliter_h

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef void* XXnaluSpliterHandle;

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * @brief    创建实例
 * @param    naluMaxLength 能够拆分存储的最大nalu数量
 * @return   失败返回NULL
 */
XXnaluSpliterHandle xxnaluSpliter_create(uint8_t naluMaxLength);

/**
 * @brief    nalu拆分，并将被拆分的nalu信息保存
 * @param    handle 对象句柄
 * @param    bufferRef 待拆分的数据
 * @param    length 待拆分的数据长度
 * @param    isH264 是否为h264
 * @return   返回被拆分的nalu数量
*/
int xxnaluSpliter_split(XXnaluSpliterHandle handle, uint8_t *bufferRef, int length, bool isH264);

/**
 * @brief    判断当前存储的nalu类型
 * @param    handle 对象句柄
 * @param    vps 输出是否包含vps
 * @param    sps 输出是否包含sps
 * @param    pps 输出是否包含pps
 * @param    iframe 输出是否包含I帧
 * @param    pframe 输出是否包含P帧
*/
void xxnaluSpliter_contains(XXnaluSpliterHandle handle, bool *vps, bool *sps, bool *pps, bool *iframe, bool *pframe);

/**
 * @brief    引用对象中已拆分的vps
 * @param    handle 对象句柄
 * @param    ref 引用指针（注意：权限是const，外部只读，不能对其数据内容修改）
 * @param    length 引用数据块长度
 * @param    containsStartCode 数据包含【0x00 00 00 01】，会影响数据以及其长度，YES：包含，NO：不包含
*/
void xxnaluSpliter_vpsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);

/**
 * @brief    引用对象中已拆分的sps
 * @param    handle 对象句柄
 * @param    ref 引用指针（注意：权限是const，外部只读，不能对其数据内容修改）
 * @param    length 引用数据块长度
 * @param    containsStartCode 数据包含【0x00 00 00 01】，会影响数据以及其长度，YES：包含，NO：不包含
*/
void xxnaluSpliter_spsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);

/**
 * @brief    引用对象中已拆分的pps
 * @param    handle 对象句柄
 * @param    ref 引用指针（注意：权限是const，外部只读，不能对其数据内容修改）
 * @param    length 引用数据块长度
 * @param    containsStartCode 数据包含【0x00 00 00 01】，会影响数据以及其长度，YES：包含，NO：不包含
*/
void xxnaluSpliter_ppsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);

/**
 * @brief    引用对象中已拆分的I帧
 * @param    handle 对象句柄
 * @param    ref 引用指针（注意：权限是const，外部只读，不能对其数据内容修改）
 * @param    length 引用数据块长度
 * @param    containsStartCode 数据包含【0x00 00 00 01】，会影响数据以及其长度，YES：包含，NO：不包含
*/
void xxnaluSpliter_iframeRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);

/**
 * @brief    引用对象中已拆分的P帧
 * @param    handle 对象句柄
 * @param    ref 引用指针（注意：权限是const，外部只读，不能对其数据内容修改）
 * @param    length 引用数据块长度
 * @param    containsStartCode 数据包含【0x00 00 00 01】，会影响数据以及其长度，YES：包含，NO：不包含
*/
void xxnaluSpliter_pframeRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);

/**
 * @brief    对象数据释放，并将句柄值置为NULL
 * @param    handlePtr 对象句柄地址
*/
void xxnaluSpliter_free(XXnaluSpliterHandle *handlePtr);
#ifdef __cplusplus
}
#endif

#endif /* XXnaluSpliter_h */
