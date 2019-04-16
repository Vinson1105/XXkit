#include "XXbyteOrder.h"

// 大端模式
uint64_t beToU64(const int8_t *data){
    uint64_t ret = 0;
    for (int i = 0; i < 8; i++){
        ret |= ((uint64_t)(data[i]) << (8 * (7 - i))) & (0x00000000000000ff << (8 * (7 - i)));
    }
    return ret;
}
uint32_t beToU32(const int8_t *data){
    uint32_t ret = 0;
    for (int i = 0; i < 4; i++){
        ret |= ((uint32_t)(data[i]) << (8 * (3 - i))) & (0x000000ff << (8 * (3 - i)));
    }
    return ret;
}
uint32_t beToU24(const int8_t *data){
    uint32_t ret = 0;
    for (int i = 0; i < 3; i++){
        ret |= ((uint32_t)data[i] << (8 * (2 - i)))  & (0x000000ff << (8 * (2 - i)));
    }
    return ret;
}
uint16_t beToU16(const int8_t *data)
{
    uint16_t ret = 0;
    for (int i = 0; i < 2; i++)
    {
        ret |= ((uint16_t)data[i] << (8 * (1 - i))) & (0x00ff << (8 * (1 - i)));
    }
    return ret;
}

// 小端模式
 uint64_t leToU64(const int8_t *data){
     uint64_t ret = 0;
     for (int i = 0; i < 8; i++){
         ret |= (uint64_t)data[i] << (8 * i);
     }
     return ret;
 }
 uint32_t leToU32(const int8_t *data){
     uint32_t ret = 0;
     for (int i = 0; i < 4; i++){
         ret |= (uint32_t)data[i] << (8 * i);
     }
     return ret;
 }
 uint32_t leToU24(const int8_t *data){
     uint32_t ret = 0;
     for (int i = 0; i < 3; i++){
         ret |= (uint32_t)data[i] << (8 * i);
     }
     return ret;
 }
 uint16_t leToU16(const int8_t *data){
     uint16_t ret = 0;
     for (int i = 0; i < 2; i++){
         ret |= (uint16_t)data[i] << (8 * i);
     }
     return ret;
 }
