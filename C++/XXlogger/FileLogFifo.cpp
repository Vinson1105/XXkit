#include "FileLogFifo.h"
#include "XXlogger.h"
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

#include <QFile>

const char * const FileLogFifo::kLogDir = "logDir";

FileLogFifo::FileLogFifo(const QVariantMap &param, QObject *parent)
    : XXfifoBase (parent)
    , _file(nullptr){
    qRegisterMetaType<QSharedPointer<QByteArray>>("QSharedPointer<QByteArray>");

    this->moveToThread(&_logThread);
    _logThread.start();
    connect(this, SIGNAL(sigPush(QSharedPointer<QByteArray>)), this, SLOT(onPush(QSharedPointer<QByteArray>)), Qt::QueuedConnection);
    connect(this, SIGNAL(sigReset(QVariantMap)), this, SLOT(onReset(QVariantMap)), Qt::QueuedConnection);

    emit sigReset(param);
}
FileLogFifo::~FileLogFifo(){
    _logThread.quit();
    if(nullptr != _file){
        _file->close();
        _file->deleteLater();
        _file = nullptr;
    }
}

void FileLogFifo::reset(const QVariantMap &param){
    emit sigReset(param);
}
void FileLogFifo::push(const QByteArray &data){
    emit sigPush(QSharedPointer<QByteArray>(new QByteArray(data)));
}

QFile* FileLogFifo::openFile(const QString &dirPath, const QString &fileName){
    if(!QDir().mkpath(dirPath)){
        xxLogWithout(this,"failure to create log dir. path:%s", dirPath.toUtf8().data());
        return nullptr;
    }

    QString filePath = "";
    if(dirPath.endsWith('/')){
        filePath = dirPath + fileName;
    }
    else{
        filePath = dirPath + '/' + fileName;
    }
    QFile *file = new QFile(filePath);
    if (!file->open(QFile::WriteOnly|QFile::Text|QFile::Append)) {
        xxLogWithout(this,"failure to open file. path:%s", filePath.toLocal8Bit().data());
        delete file;
        return nullptr;
    }
    //file->close();  // 为什么这里需要close一下？因为写入文件是在另一个线程中，那么需要在另一个线程中打开
    return file;
}
void FileLogFifo::onPush(QSharedPointer<QByteArray> bytes){
    if(nullptr == _file){
        return;
    }

    if(_file->write(*bytes) <= 0){
        // 注意：这里写入时需要排除自身，否则存在死循环的情况！
        xxLogWithout(this,"failure to write");
    }
    _file->flush();
}
void FileLogFifo::onReset(QVariantMap param){
    if(nullptr != _file){
        _file->close();
        _file->deleteLater();
        _file = nullptr;
    }

    if(param.contains(kLogDir)){
        _file = openFile(param[kLogDir].toString(),QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    }
    else{
        xxLogWithout(this,"param not contains \"%s\"", kLogDir);
    }
}

