#ifndef XXfmH26xDecoder_h
#define XXfmH26xDecoder_h

#include "../XXavUtility.h"

typedef void* XXfmH26xDecoderHandle;
typedef struct XXfmH26xDecoderOutput{
    int width;
    int height;
    uint8_t *data[3];
    int length[3];
}XXfmH26xDecoderOutput;

#ifdef __cplusplus
extern "C"{
#endif	// __cplusplus
    
    XXfmH26xDecoderOutput* XXfmH26xDecoderOutput_alloc(void);
    void XXfmH26xDecoderOutput_free(XXfmH26xDecoderOutput **output);
    
	XXfmH26xDecoderHandle xxfmH264Decoder_create(void);
    XXfmH26xDecoderHandle xxfmH265Decoder_create(void);

    int xxfmH26xDecoder_decode(XXfmH26xDecoderHandle handle, const uint8_t *data, int length, XXfmH26xDecoderOutput *output);
    void xxfmH26xDecoder_free(XXfmH26xDecoderHandle *handle);

#ifdef __cplusplus
}
#endif	// __cplusplus
#endif	// XXfmH26xDecoder_h
