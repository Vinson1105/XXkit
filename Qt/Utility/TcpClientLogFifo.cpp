#include "TcpClientLogFifo.h"

TcpClientLogFifo::TcpClientLogFifo(const QVariantMap &param, QObject *parent)
    : XXfifoBase(parent)
    , _socket(nullptr)
    , _ready(false){
    qRegisterMetaType<QSharedPointer<QByteArray>>("QSharedPointer<QByteArray>");

    this->moveToThread(&_logThread);
    _logThread.start();
    connect(this, SIGNAL(sigPush(QSharedPointer<QByteArray>)), this, SLOT(onPush(QSharedPointer<QByteArray>)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigReset(QVariantMap)), this, SLOT(onReset(QVariantMap)), Qt::QueuedConnection);

    emit sigReset(param);
}
TcpClientLogFifo::~TcpClientLogFifo(){
    _logThread.requestInterruption();
    _logThread.quit();
    if(nullptr != _socket){
        _socket->disconnectFromHost();
        _socket->deleteLater();
        _socket = nullptr;
    }
}

void TcpClientLogFifo::reset(const QVariantMap &param){
    emit sigReset(param);
}
void TcpClientLogFifo::push(const QByteArray &data){
    emit sigPush(QSharedPointer<QByteArray>(new QByteArray(data)));
}


QTcpSocket* TcpClientLogFifo::connectToServer(const QString &ip, int port){
    QTcpSocket *socket = new QTcpSocket;
    connect(socket,SIGNAL(connected()),this,SLOT(socket_onConnected()),Qt::QueuedConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(socket_onDisconnected()),Qt::QueuedConnection);
    socket->connectToHost(ip,port);
    return socket;
}
void TcpClientLogFifo::disconnectFromServer(QTcpSocket *socket){
    // 这里直接ready手动设置为false，而不是通过socket的disconnect
    /**
     * ready变量是由槽函数的
     * 这里直接ready手动设置为false，而不是通过socket的disconnect，
     *
     */
    _ready = false;

    disconnect(socket,SIGNAL(connected()),this,SLOT(socket_onConnected()));
    disconnect(socket,SIGNAL(disconnected()),this,SLOT(socket_onDisconnected()));
    socket->disconnectFromHost();
    socket->deleteLater();
}

void onPush(QSharedPointer<QByteArray> bytes);
void onReset(QVariantMap param);

/**
 * Tcp的一些信号-槽
 */
void socket_onConnected();
void socket_onDisconnected();
