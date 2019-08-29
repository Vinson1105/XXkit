#ifndef XXnet_h
#define XXnet_h

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    /**
     * @brief   Host地址转换,如在IPv6的环境下,将目标IPv4地址转换成IPv6格式
     * @param    dest 转换后的buffer
     * @param    isIPv6 返回该地址是否为IPv6
     * @return   返回0成功,否则失败
     */
    bool xxnet_transformHost(char *dest, bool *isIPv6, char *src, int port);
    
    /**
     * @brief   判断sockaddr是否为IPv6类型
     * @param   address 需要判断的sockaddr目标实例
     * @return  返回true为IPv6
     */
    bool xxnet_isIPv6(struct sockaddr *address);
    
    /**
     * @brief   从sockaddr(ipv4)中取回host字符串
     * @param   host 输出host的buffer
     * @param   address 源地址实例
     * @return  返回0成功,否则失败
     */
    bool xxnet_hostFromAddress4(char *host, struct sockaddr_in *address);
    
    /**
     * @brief   从sockaddr(ipv4)中取回port
     * @param   address 源地址实例
     * @return  返回port
     */
    int xxnet_portFromAddress4(struct sockaddr_in *address);
    
    /**
     * @brief   从sockaddr(ipv6)中取回host字符串
     * @param   host 输出host的buffer
     * @param   address 源地址实例
     * @return  返回0成功,否则失败
     */
    bool xxnet_hostFromAddress6(char *host, struct sockaddr_in6 *address);
    
    /**
     * @brief   从sockaddr(ipv6)中取回port
     * @param   address 源地址实例
     * @return  返回port
     */
    int xxnet_portFromAddress6(struct sockaddr_in6 *address);
    
    /**
     * @brief   从sockaddr取回ip和port
     * @param   host host的输出buffer指针
     * @param   port port输出指针
     * @param   isIPv6 是否为IPv6地址
     * @param   address 目标address
     */
    void xxnet_infoFromAddress(char *host, int *port, bool *isIPv6, struct sockaddr *address);
    
    /**
     * @brief   初始化sockaddr(IPv4)
     * @param   host IP地址
     * @param   port 端口
     */
    void xxnet_address4(struct sockaddr_in *address, const char *host, int port);
    
    /**
     * @brief   初始化sockaddr(IPv6)
     * @param   host IP地址
     * @param   port 端口
     */
    void xxnet_address6(struct sockaddr_in6 *address, const char *host, int port);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XXnet_h */
