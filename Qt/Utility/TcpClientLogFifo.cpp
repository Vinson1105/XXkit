#include "TcpClientLogFifo.h"
#include "XXlogger.h"

const char * const TcpClientLogFifo::kLogServerIP = "ip";
const char * const TcpClientLogFifo::kLogServerPort = "port";

TcpClientLogFifo::TcpClientLogFifo(const QVariantMap &param, QObject *parent)
    : XXfifoBase(parent)
    , _socket(nullptr){
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
        disconnectFromServer(_socket);
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
    disconnect(socket,SIGNAL(connected()),this,SLOT(socket_onConnected()));
    disconnect(socket,SIGNAL(disconnected()),this,SLOT(socket_onDisconnected()));
    socket->disconnectFromHost();
    socket->deleteLater();
}
bool TcpClientLogFifo::isSocketReady(QTcpSocket *socket){
    return nullptr != socket && socket->state() != QAbstractSocket::SocketState::ConnectedState;
}

void TcpClientLogFifo::onPush(QSharedPointer<QByteArray> bytes){
    if(!isSocketReady(_socket)){
        return;
    }
    _socket->write(*bytes);
}
void TcpClientLogFifo::onReset(QVariantMap param){
    if(nullptr != _socket){
        disconnectFromServer(_socket);
        _socket = nullptr;
    }

    QString ip = param.contains(kLogServerIP) ? param[kLogServerIP].toString() : "";
    int port = param.contains(kLogServerPort) ? param[kLogServerPort].toInt() : 0;
    if(ip.isEmpty() || port <= 0){
        xxLogStrWithout(this,QString("invalid param. ip:%1 port:%2").arg(ip).arg(port));
        return;
    }

    _socket = connectToServer(ip, port);
}

/**
 * Tcp的一些信号-槽
 */
void TcpClientLogFifo::socket_onConnected(){

}
void TcpClientLogFifo::socket_onDisconnected(){

}
