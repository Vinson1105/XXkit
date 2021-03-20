#ifndef XXLOGGER_H
#define XXLOGGER_H

#include <QObject>
#include <QVariant>
#include <QSharedPointer>

class XXfifoBase : public QObject{
    Q_OBJECT
public:
    XXfifoBase(QObject *parent = nullptr) : QObject(parent){}
    virtual ~XXfifoBase() {}

    virtual void push(const QByteArray &data) = 0;
    virtual bool isEnabled() const { return _enable; }
    virtual void setEnable(bool enable){ _enable = enable; }

protected:
    bool _enable;
};

class XXlogger : public QObject{
    Q_OBJECT
public:
    XXlogger() {}
    virtual ~XXlogger() {}

    void message(const char *format, ...);
    void message(XXfifoBase *without, const char *format, ...);
    void configFifo(const QString &name, XXfifoBase *fifo);
    void setFifoEnable(const QString &name, bool enable);

    static XXlogger* instance();

private:
    QMap<QString,QSharedPointer<XXfifoBase>> _nameToFifo;
    static XXlogger *_instance;
};

#endif // XXLOGGER_H
