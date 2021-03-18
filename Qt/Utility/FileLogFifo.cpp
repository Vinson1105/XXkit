#include "FileLogFifo.h"

FileLogFifo::FileLogFifo(const QVariantMap &param)
    : XXfifoBase (){

}

XXfifoBase* FileLogFifo::create(const QVariantMap &param) const{
    return new FileLogFifo(param);
}
void FileLogFifo::push(const QByteArray &data){

}
