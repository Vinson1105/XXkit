#ifndef FILELOGFIFO_H
#define FILELOGFIFO_H

#include "XXlogger.h"
#include <QFile>

class FileLogFifo : public XXfifoBase{
public:
    FileLogFifo(const QVariantMap &param);
    ~FileLogFifo();

    XXfifoBase* create(const QVariantMap &param) const;
    void push(const QByteArray &data);

private:
    QFile *_file;
};

#endif // FILELOGFIFO_H
