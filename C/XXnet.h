#ifndef XXnet_h
#define XXnet_h

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    bool xxnet_isIPV4(const char *ip);
    bool xxnet_isIPV6(const char *ip);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XXnet_h */
