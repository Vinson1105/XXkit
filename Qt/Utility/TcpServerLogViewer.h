#ifndef TCPSERVERLOGVIEWER_H
#define TCPSERVERLOGVIEWER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTcpSocket>

namespace Ui {
class TcpServerLogViewer;
}

class TcpServerLogViewer : public QMainWindow{
    Q_OBJECT

public:
    explicit TcpServerLogViewer(QWidget *parent = nullptr);
    ~TcpServerLogViewer();

private slots:
    void tcpServer_onNewConnection();
    void tcpClient_onConnected();
    void tcpClient_onDisconnected();
    void tcpClient_onReadReady();

private:
    Ui::TcpServerLogViewer *ui;

    QTcpServer *_server;
    QList<QSharedPointer<QTcpSocket>> _clients;
};

#endif // TCPSERVERLOGVIEWER_H
