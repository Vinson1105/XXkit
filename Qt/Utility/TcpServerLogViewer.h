#ifndef TCPSERVERLOGVIEWER_H
#define TCPSERVERLOGVIEWER_H

#include <QMainWindow>

namespace Ui {
class TcpServerLogViewer;
}

class TcpServerLogViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpServerLogViewer(QWidget *parent = nullptr);
    ~TcpServerLogViewer();

private:
    Ui::TcpServerLogViewer *ui;
};

#endif // TCPSERVERLOGVIEWER_H
