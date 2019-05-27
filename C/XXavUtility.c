#include "XXavUtility.h"
#include <stdlib.h>

XXavFrame* xxavFrame_alloc(void){
    XXavFrame *frame = (XXavFrame*)malloc(sizeof(XXavFrame));
    frame->data     = NULL;
    frame->length   = 0;
    return frame;
}
void xxavFrame_free(XXavFrame **frame){
    if (NULL == frame || NULL == *frame) {
        return ;
    }
    
    if (NULL != (*frame)->data) {
        free((*frame)->data);
    }
    free(*frame);
    *frame = NULL;
}
