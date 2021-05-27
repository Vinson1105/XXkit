//
//  XXfmH26xEncoder.h
//  NVSDK
//
//  Created by VINSON on 2020/8/10.
//  Copyright © 2020 macrovideo. All rights reserved.
//

#ifndef XXfmH26xEncoder_h
#define XXfmH26xEncoder_h

#include <stdio.h>
#include <stdbool.h>

typedef void* XXfmH26xEncoderHandle;
typedef enum XXfmH26xEncoderFormatType{
    XXfmH26xEncoderFormatTypeRgb24 = 1,
    XXfmH26xEncoderFormatTypeYuv420,
}XXfmH26xEncoderFormatType;

#ifdef __cplusplus
extern "C"{
#endif    // __cplusplus

    XXfmH26xEncoderHandle xxfmH264Encoder_create(int width, int height, XXfmH26xEncoderFormatType format);
    XXfmH26xEncoderHandle xxfmH265Encoder_create(int width, int height,XXfmH26xEncoderFormatType format);

    int xxfmH26xEncoder_encode(XXfmH26xEncoderHandle handle,
                               const uint8_t *srcData,
                               int srcLength,
                               const uint8_t *destData,
                               int *destLength,
                               bool *keyFrame);
    void xxfmH26xEncoder_free(XXfmH26xEncoderHandle *handle);

#ifdef __cplusplus
}
#endif    // __cplusplus
#endif    // XXfmH26xEncoder_h
