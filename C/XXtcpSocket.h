#ifndef XXtcpSocket_h
#define XXtcpSocket_h

#include <stdio.h>
#include <stdbool.h>

#define XXSOCKET_ERROR_UNDEFINED    -1
#define XXSOCKET_ERROR_TIMEOUT      -2
#define XXSOCKET_ERROR_CLOSED       -3

typedef void* XXtcpSocketHandle;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    /**
     * @brief    分配句柄资源
     * @param    recvBufferSize socket接收缓冲区大小
     * @param    recvTryTimes socket进行recv的时候尝试次数
     * @param    waitSelectMsec socket进行select的等待时长,需要需要传入毫秒
     * @return   返回NULL发生错误,否则成功
     */
    XXtcpSocketHandle xxtcpSocket_alloc(int recvBufferSize, int recvTryTimes, time_t waitSelectMsec);
    
    /**
     * @brief    连接到指定的ip和port,兼容ipv4和ipv6
     * @param    handle 目标句柄
     * @param    ip 目标ip
     * @param    port 目标port
     * @return   返回0成功,否则失败
     */
    int xxtcpSocket_connectToHost(XXtcpSocketHandle handle, const char *ip, int port);
    
    /**
     * @brief    发送数据
     * @param    handle 目标句柄
     * @param    data 数据内容起始地址
     * @param    length 数据内容长度
     * @return   返回>=0实际发送数据长度,否则失败
     */
    ssize_t xxtcpSocket_send(XXtcpSocketHandle handle, uint8_t *data, ssize_t length);
    
    /**
     * @brief    接收数据
     * @param    handle 目标句柄
     * @param    data 用于存储接收的数据
     * @param    maxLength 最大接收长度
     * @return   返回>=0实际接收数据长度,否则失败
     */
    ssize_t xxtcpSocket_recv(XXtcpSocketHandle handle, uint8_t *data, ssize_t length);
    
    /**
     * @brief    按照指定的尝试次数和单次等待时间来接收数据
     * @param    handle 目标句柄
     * @param    data 用于存储接收的数据
     * @param    maxLength 最大接收长度
     * @param    times 循环接收次数
     * @param    waitSelectMsec 单次循环select等待时间
     * @return   返回>=0实际接收数据长度,否则失败
     */
    ssize_t xxtcpSocket_waitForRecv(XXtcpSocketHandle handle, uint8_t *data, ssize_t maxLength, int times, time_t waitSelectMsec);
    
    /**
     * @brief    断开连接
     * @param    handle 目标句柄
     */
    void xxtcpSocket_disconnect(XXtcpSocketHandle handle);
    
    /**
     * @brief    释放资源,handle会被置NULL
     * @param    handle 目标句柄地址
     */
    void xxtcpSocket_free(XXtcpSocketHandle *handle);
    
    /** 用于测试 */
    int xxtcpSocket_socket(XXtcpSocketHandle handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* XXtcpSocket_h */
