#ifndef TCPCLIENTLOGFIFO_H
#define TCPCLIENTLOGFIFO_H

#include "XXlogger.h"
#include <QTcpSocket>

class TcpClientLogFifo : public XXfifoBase{
    Q_OBJECT
public:
    TcpClientLogFifo(const QVariantMap &param, QObject *parent = nullptr);
    ~TcpClientLogFifo();

    void reset(const QVariantMap &param);
    void push(const QByteArray &data);

public: // 一些公开常量
    static const char * const kLogServerIP;
    static const char * const kLogServerPort;
    static const char * const kLogClientName;

private:
    QTcpSocket *connectToServer(const QString &ip, int port);
    void disconnectFromServer(QTcpSocket *socket);

    /**
     * 由于push等函数可能存在多线程调用的情况，那么在多线程下操作socket可能会导致一些异常情况；
     * 这里就采取了信号-槽处理，槽直接放到固定线程中，使用事件循环做一个写触发队列；
     */
    Q_SIGNAL void sigPush(QSharedPointer<QByteArray> bytes);
    Q_SIGNAL void sigReset(QVariantMap param);

    Q_SLOT void onPush(QSharedPointer<QByteArray> bytes);
    Q_SLOT void onReset(QVariantMap param);

    /**
     * Tcp的一些信号-槽
     */
    Q_SLOT void socket_onConnected();
    Q_SLOT void socket_onDisconnected();

private:
    bool _ready;
    QTcpSocket *_socket;   // socket客户端
    QThread _logThread;   // socket操作的线程
};

#endif // TCPCLIENTLOGFIFO_H
