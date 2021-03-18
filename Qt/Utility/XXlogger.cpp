#include "XXlogger.h"
#include <QString>
#include <QDateTime>
#include <QSharedPointer>

#define ASPRINTF(format,...) (QString::asprintf(format,##__VA_ARGS__))

XXlogger* XXlogger::_instance = nullptr;

void XXlogger::message(const char *format,...){
    if(nullptr == format){
        return;
    }
    va_list args;
    va_start(args, format);
    QString msg = QString::asprintf("[%s] [%s] [%d] ", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.sss").toUtf8().data(), __FUNCTION__, __LINE__)+QString::asprintf(format,args);
    va_end(args);

    QByteArray bytes = msg.toUtf8();
    for (auto iter=_nameToFifo.cbegin(); iter!=_nameToFifo.cend(); iter++) {
        if(!(*iter)->isEnabled()){
            continue;
        }
        (*iter)->push(bytes);
    }
}
void XXlogger::configFifo(const QString &name, const XXfifoBase &fifo, const QVariantMap &param){
    _nameToFifo[name] = QSharedPointer<XXfifoBase>(fifo.create(param));
}
void XXlogger::setFifoEnable(const QString &name, bool enable){
    if(!_nameToFifo.contains(name)){
        return;
    }
    _nameToFifo[name]->setEnable(enable);
}

XXlogger* XXlogger::instance(){
    if(nullptr == _instance){
        _instance = new XXlogger;
    }
    return _instance;
}
