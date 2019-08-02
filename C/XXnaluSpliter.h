#ifndef XXnaluSpliter_h
#define XXnaluSpliter_h

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef void* XXnaluSpliterHandle;

#ifdef __cplusplus
extern "C" {
#endif
    XXnaluSpliterHandle xxnaluSpliter_create(uint8_t naluMaxLength);
    int xxnaluSpliter_split(XXnaluSpliterHandle handle, uint8_t *bufferRef, int length, bool isH264);
    
    void xxnaluSpliter_contains(XXnaluSpliterHandle handle, bool *vps, bool *sps, bool *pps, bool *iframe, bool *pframe);
    void xxnaluSpliter_vpsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);
    void xxnaluSpliter_spsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);
    void xxnaluSpliter_ppsRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);
    void xxnaluSpliter_iframeRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);
    void xxnaluSpliter_pframeRef(XXnaluSpliterHandle handle, uint8_t **ref, uint32_t *length, bool containsStartCode);

    void xxnaluSpliter_free(XXnaluSpliterHandle *handlePtr);
#ifdef __cplusplus
}
#endif

#endif /* XXnaluSpliter_h */
