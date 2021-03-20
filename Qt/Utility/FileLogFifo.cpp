#include "FileLogFifo.h"
#include "XXlogger.h"
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

#include <QFile>

const char * const FileLogFifo::kLogDir = "LogDir";

FileLogFifo::FileLogFifo(const QVariantMap &param, QObject *parent)
    : XXfifoBase (parent)
    , _file(nullptr){
    if(param.contains(kLogDir)){
        _file = openFile(param[kLogDir].toString(),QDateTime::currentDateTime().toString("yyyy-MM-hh"));
    }
    else{
        XXlogger::instance()->message("param not contains \"%s\".", kLogDir);
    }
    this->moveToThread(&_writeThread);
    _writeThread.start();
    connect(this, SIGNAL(sigPush(QSharedPointer<QByteArray>)), this, SLOT(onPush(QSharedPointer<QByteArray>)), Qt::QueuedConnection);
}
FileLogFifo::~FileLogFifo(){
    _writeThread.quit();
    if(_file){
        _file->close();
        _file->deleteLater();
        _file = nullptr;
    }
}

void FileLogFifo::push(const QByteArray &data){
    emit sigPush(QSharedPointer<QByteArray>(new QByteArray(data)));
}

QFile* FileLogFifo::openFile(const QString &dirPath, const QString &fileName){
    QFileInfo dirInfo(dirPath);
    if(!dirInfo.exists() || dirInfo.isFile()){
        XXlogger::instance()->message("log dir path is a file. path:%s",dirPath.toLocal8Bit().data());
        return nullptr;
    }

    if(!QDir().mkpath(dirPath)){
        XXlogger::instance()->message("failure to create log dir. path:%s", dirPath.toLocal8Bit().data());
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
    if (!file->open(QFile::ReadWrite|QFile::Text|QFile::Append)) {
        XXlogger::instance()->message("failure to open file. path:%s", filePath.toLocal8Bit().data());
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

    if(_file->write(*bytes) < 0){
        // 注意：这里写入时需要排除自身，否则存在死循环的情况！
        XXlogger::instance()->message(this, "failure to write.");
    }
}

