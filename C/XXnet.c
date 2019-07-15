#include "XXnet.h"
#include <arpa/inet.h>

bool xxnet_isIPV4(const char *ip){
    if (!ip) {
        return false;
    }
    
    if (inet_addr(ip) == -1) {
        return false;
    }
    return true;
}
bool xxnet_isIPV6(const char *ip){
    if (!ip) {
        return false;
    }
    
    struct sockaddr_in6 addr;
    if (inet_pton(AF_INET6, ip, &(addr.sin6_addr)) == 0) {
        return false;
    }
    return true;
}
