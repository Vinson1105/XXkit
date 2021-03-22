#include "XXlogger.h"
#include <QString>
#include <QDateTime>
#include <QSharedPointer>
#include <QDebug>

#include "FileLogFifo.h"
#include <QCoreApplication>
#include <QDir>
#include <QCoreApplication>

#define ASPRINTF(format,...) (QString::asprintf(format,##__VA_ARGS__))

XXlogger* XXlogger::_instance = nullptr;
const char* const XXlogger::kDefaultQDebugFifoName = "qDebugFifo";
const char* const XXlogger::kDefaultFileFifoName = "fileFifo";

XXlogger::XXlogger()
    : _qDebugFifoEnable(true){
    initDefaultFifo();
}

void XXlogger::message(const QString &msg){
    this->output(msg.toUtf8());
}
void XXlogger::message(XXfifoBase *without, const QString &msg){
    this->output(msg.toUtf8(),without);
}
void XXlogger::configFifo(const QString &name, XXfifoBase *fifo){
    _nameToFifo[name] = QSharedPointer<XXfifoBase>(fifo);
    fifo->setEnable(true);
}
void XXlogger::setFifoEnable(const QString &name, bool enable){
    if(name == kDefaultQDebugFifoName){
        _qDebugFifoEnable = enable;
        return;
    }
    if(!_nameToFifo.contains(name)){
        return;
    }
    _nameToFifo[name]->setEnable(enable);
}
void XXlogger::resetFifo(const QString &name, const QVariantMap &param){
    if(name == kDefaultQDebugFifoName){
        return;
    }
    if(!_nameToFifo.contains(name)){
        return;
    }
    _nameToFifo[name]->reset(param);
}

XXlogger& XXlogger::instance(){
    if(nullptr == _instance){
        _instance = new XXlogger;
    }
    return *_instance;
}

void XXlogger::initDefaultFifo(){
    // 默认的文件fifo配置
    QString path = QDir::homePath()+"/"+QCoreApplication::applicationName()+"/xxlogger";
    FileLogFifo *fifo = new FileLogFifo({
                                            {FileLogFifo::kLogDir,path}
                                        });

    configFifo(kDefaultFileFifoName,fifo);
}
void XXlogger::output(const QString &msg, XXfifoBase *without){
    QString msgWithDateTime = QString("[%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")) + msg;
    QByteArray bytes = msgWithDateTime.toUtf8();
    for (auto iter=_nameToFifo.cbegin(); iter!=_nameToFifo.cend(); iter++) {
        if(!(*iter)->isEnabled() || without == *iter){
            continue;
        }
        (*iter)->push(bytes);
    }
    if(_qDebugFifoEnable){
        qDebug() << msg;
    }
}

