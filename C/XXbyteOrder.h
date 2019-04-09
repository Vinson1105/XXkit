/*
 * @author: GWX
 * @date: 20181229
 * @descirption: 从内存中以大端或者小端模式读取数据.
 * @history:
 */

#ifndef XXbyteOrder_h
#define XXbyteOrder_h

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
    
    uint64_t beToU64(const uint8_t *data);
    uint32_t beToU32(const uint8_t *data);
    uint32_t beToU24(const uint8_t *data);
    uint16_t beToU16(const uint8_t *data);
    
    uint64_t leToU64(const uint8_t *data);
    uint32_t leToU32(const uint8_t *data);
    uint32_t leToU24(const uint8_t *data);
    uint16_t leToU16(const uint8_t *data);
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XXbyteOrder_h
