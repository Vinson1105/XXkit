#include "XXpi.h"
#include <QDataStream>

#define kHeader "[XX]"
#define kEndl "\r\n"

QByteArray XXpi::combine(const QString &type, const QByteArray &payload){
    QByteArray bytes;
    QDataStream stream(bytes);
    stream  << kHeader kEndl
            << type << kEndl
            << payload << kEndl kEndl;
}
qint64 XXpi::parse(const QByteArray &bytes, QString &oType, QByteArray &oPayload){
    if(!bytes.startsWith(kHeader kEndl)) {
        return -1;
    }
    if(!bytes.contains(kEndl kEndl)){
        return -2;
    }

    QByteArray temp;
    for(int index=0; index<bytes.length(); index++){
        if(index)
    }
}
