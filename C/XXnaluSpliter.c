#include "XXnaluSpliter.h"
#include <stdlib.h>
#include <string.h>

#define H264_SPS_CODEC      0x07
#define H264_PPS_CODEC      0x08
#define H264_I_CODEC        0x05
#define H264_P_CODEC        0x01

#define H265_VPS_CODEC      0x20
#define H265_SPS_CODEC      0x21
#define H265_PPS_CODEC      0x22
#define H265_I_CODEC        0x13
#define H265_P_CODEC        0x01

const uint8_t START_CODE[]  = {0x00, 0x00, 0x00, 0x01};

typedef enum XXnaluType{
    XXnaluType_Unknown = 1,
    XXnaluType_VPS,
    XXnaluType_SPS,
    XXnaluType_PPS,
    XXnaluType_IFrame,
    XXnaluType_PFrame,
}XXnaluType;

typedef struct XXnalu{
    XXnaluType type;    // Nalu类型
    uint8_t *dataWithoutStartCode;      // 不包含头
    uint8_t *dataWithStartCode;         // 包含头
    uint32_t lengthWithoutStartCode;    // Nalu的长度
    uint32_t lengthWithStartCode;    // Nalu的长度
}XXnalu;

typedef struct XXnaluSpliterContext{
    XXnalu *naluArray;
    uint8_t naluCount;
    uint8_t naluArrayMaxLength;
}XXnaluSpliterContext;

static bool getNalu(XXnaluSpliterContext *context, XXnaluType type, uint8_t **ref, uint32_t *length, bool containsStartCode);
static int splitNalu(XXnalu naluList[], int naluListMax, uint8_t *data, int length, bool isH264);
static XXnaluType toXXnaluType(uint8_t *data, bool isH264);

XXnaluSpliterHandle xxnaluSpliter_create(uint8_t naluMaxLength){
    int arraySize = sizeof(XXnalu) * naluMaxLength;
    XXnalu *naluArray = (XXnalu*)malloc(arraySize);
    if(NULL == naluArray){
        return NULL;
    }
    memset(naluArray, 0, arraySize);
    
    XXnaluSpliterContext *context   = (XXnaluSpliterContext*)malloc(sizeof(XXnaluSpliterContext));
    if(NULL == context){
        free(naluArray);
        return NULL;
    }
    context->naluArray               = naluArray;
    context->naluArrayMaxLength      = naluMaxLength;
    return context;
}
int xxnaluSpliter_split(XXnaluSpliterHandle handle, uint8_t *bufferRef, int length, bool isH264){
    XXnaluSpliterContext *context = handle;
    if (NULL == context) {
        return -1;
    }
    
    memset(context->naluArray, 0, sizeof(XXnalu)*context->naluArrayMaxLength);
    int count = splitNalu(context->naluArray, context->naluArrayMaxLength, bufferRef, length, isH264);
    context->naluCount = count;
    return count;
}

void xxnaluSpliter_contains(XXnaluSpliterHandle handle, bool *vps, bool *sps, bool *pps, bool *iframe, bool *pframe){
    *vps = *sps = *pps = *iframe = *pframe = false;
    XXnaluSpliterContext *context = handle;
    if (NULL == context) {
        return;
    }
    
    int count = context->naluCount;
    for (int i = 0; i < count; i++) {
        XXnaluType type = context->naluArray[i].type;
        switch (type) {
            case XXnaluType_VPS: *vps = true; break;
            case XXnaluType_SPS: *sps = true; break;
            case XXnaluType_PPS: *pps = true; break;
            case XXnaluType_IFrame: *iframe = true; break;
            case XXnaluType_PFrame: *pframe = true; break;
            default: break;
        }
    }
}
void xxnaluSpliter_vpsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode){
    *ref    = NULL;
    *length = 0;
    XXnaluSpliterContext *context = handle;
    if (NULL == context) {
        return;
    }
    getNalu(context, XXnaluType_VPS, ref, length, containsStartCode);
}
void xxnaluSpliter_spsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode){
    *ref    = NULL;
    *length = 0;
    XXnaluSpliterContext *context = handle;
    if (NULL == context) {
        return;
    }
    getNalu(context, XXnaluType_SPS, ref, length, containsStartCode);
}
void xxnaluSpliter_ppsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode){
    *ref    = NULL;
    *length = 0;
    XXnaluSpliterContext *context = handle;
    if (NULL == context) {
        return;
    }
    getNalu(context, XXnaluType_PPS, ref, length, containsStartCode);
}
void xxnaluSpliter_iframeRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode){
    *ref    = NULL;
    *length = 0;
    XXnaluSpliterContext *context = handle;
    if (NULL == context) {
        return;
    }
    getNalu(context, XXnaluType_IFrame, ref, length, containsStartCode);
}
void xxnaluSpliter_pframeRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode){
    *ref    = NULL;
    *length = 0;
    XXnaluSpliterContext *context = handle;
    if (NULL == context) {
        return;
    }
    getNalu(context, XXnaluType_PFrame, ref, length, containsStartCode);
}

void xxnaluSpliter_free(XXnaluSpliterHandle *handlePtr){
    if (NULL == handlePtr || NULL == *handlePtr) {
        return;
    }
    XXnaluSpliterContext **context = (XXnaluSpliterContext**)handlePtr;
    free((*context)->naluArray);
    (*context)->naluArray = NULL;
    free(*context);
    *context = NULL;
}

static bool getNalu(XXnaluSpliterContext *context, XXnaluType type, uint8_t **ref, uint32_t *length, bool containsStartCode){
    int count = context->naluCount;
    for (int i = 0; i < count; i++) {
        XXnalu nalu = context->naluArray[i];
        if(type == nalu.type){
            *ref    = containsStartCode ? nalu.dataWithStartCode : nalu.dataWithoutStartCode;
            *length = containsStartCode ? nalu.lengthWithStartCode : nalu.lengthWithoutStartCode;
            return true;
        }
    }
    return false;
}
static int splitNalu(XXnalu naluList[], int naluListMax, uint8_t *data, int length, bool isH264){
    uint8_t naluCount           = 0;
    int index                   = 0;
    int previousNaluOffset      = -1;
    int currentNaluOffset       = -1;
    
    while (index < length - 4) {
        if(0 == memcmp(START_CODE, data+index, 4)){
            index += 4;
            if (naluCount >= naluListMax) {
                break;
            }
            
            currentNaluOffset                           = index;                                // 记录nale数据起始偏移
            naluList[naluCount].dataWithStartCode       = data + currentNaluOffset - 4;
            naluList[naluCount].dataWithoutStartCode    = data + currentNaluOffset;             // 记录nale数据起始地址,注意:此处直接使用源数据的地址
            naluList[naluCount].lengthWithStartCode     = 0;                                    // 由于本nalu的长度需要获取下一个nalu的startCode位置才可以计算,所以先置为0
            naluList[naluCount].lengthWithoutStartCode  = 0;                                    // 由于本nalu的长度需要获取下一个nalu的startCode位置才可以计算,所以先置为0
            naluList[naluCount].type                    = toXXnaluType(data + index, isH264);   // 将nale类型转换到统一的类型定义
            if (naluCount > 0) {
                naluList[naluCount - 1].lengthWithStartCode     = currentNaluOffset - previousNaluOffset;       // 计算上一个nale的数据长度
                naluList[naluCount - 1].lengthWithoutStartCode  = currentNaluOffset - previousNaluOffset - 4;   // 计算上一个nale的数据长度
            }
            
            previousNaluOffset  = currentNaluOffset;
            naluCount           += 1;
        }
        else{
            index++;
        }
    }
    
    if (naluCount > 0) {
        naluList[naluCount - 1].lengthWithStartCode     = length - previousNaluOffset + 4;  // 记录上一个nalu的长度
        naluList[naluCount - 1].lengthWithoutStartCode  = length - previousNaluOffset;      // 记录上一个nalu的长度
    }
    return naluCount;
}
static XXnaluType toXXnaluType(uint8_t *data, bool isH264){
    if(isH264){
        uint8_t type = *data & 0x1f;
        switch (type) {
            case H264_SPS_CODEC:    return XXnaluType_SPS;
            case H264_PPS_CODEC:    return XXnaluType_PPS;
            case H264_I_CODEC:      return XXnaluType_IFrame;
            case H264_P_CODEC:      return XXnaluType_PFrame;
            default:                return XXnaluType_Unknown;
        }
    }
    else{
        uint8_t type = (*data >> 1) & 0x3f;
        switch (type) {
            case H265_VPS_CODEC:    return XXnaluType_VPS;
            case H265_SPS_CODEC:    return XXnaluType_SPS;
            case H265_PPS_CODEC:    return XXnaluType_PPS;
            case H265_I_CODEC:      return XXnaluType_IFrame;
            case H265_P_CODEC:      return XXnaluType_PFrame;
            default:                return XXnaluType_Unknown;
        }
    }
}
