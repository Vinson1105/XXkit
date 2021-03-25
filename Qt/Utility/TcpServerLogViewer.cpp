#include "TcpServerLogViewer.h"
#include "ui_TcpServerLogViewer.h"

TcpServerLogViewer::TcpServerLogViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpServerLogViewer)
{
    ui->setupUi(this);
}

TcpServerLogViewer::~TcpServerLogViewer()
{
    delete ui;
}
