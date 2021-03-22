#ifndef XXLOGGER_H
#define XXLOGGER_H

#include <QObject>
#include <QVariant>
#include <QSharedPointer>

#define XXlog(format,args...) \
    {\
        QString msg = QString("[%1] [%2] %3\n").arg(__FUNCTION__).arg(__LINE__).arg(QString::asprintf(format,##args)); \
        XXlogger::instance().message(msg); \
    }
#define XXlogWithout(fifo,format,args...) \
    {\
        QString msg = QString("[%1] [%2] %3\n").arg(__FUNCTION__).arg(__LINE__).arg(QString::asprintf(format,##args)); \
        XXlogger::instance().message(fifo,msg); \
    }

class XXfifoBase : public QObject{
    Q_OBJECT
public:
    XXfifoBase(QObject *parent = nullptr) : QObject(parent){}
    virtual ~XXfifoBase() {}

    virtual void reset(const QVariantMap &param) = 0;
    virtual void push(const QByteArray &data) = 0;
    virtual bool isEnabled() const { return _enable; }
    virtual void setEnable(bool enable){ _enable = enable; }

protected:
    bool _enable;
};

/**
 * @brief The XXlogger class
 * 默认配置了以下fifo
 *  - qDebug，name=“qDebugFifo”
 *  - file，name=“fileFifo”，默认路径为/「home」/「App」/xxlogger，文件格式为「yyyy-MM-dd」
 */
class XXlogger : public QObject{
    Q_OBJECT
public:
    XXlogger();
    virtual ~XXlogger() {}

    /**
     * @brief message 信息输出
     * @param msg 需要输出的信息
     */
    void message(const QString &msg);

    /**
     * @brief message 信息输出，并可以过滤指定的fifo
     * @param msg 需要输出的信息
     */
    void message(XXfifoBase *without, const QString &msg);

    /**
     * @brief configFifo 配置目标fifo（需要继承XXfifoBase），并移交释放权到XXlogger中，name相当是对应fifo的控制key；
     * @param name fifo的别名，不要与默认别名重复，默认fifo别名有：kdefaultQDebugFifoName和kDefaultFileFifoName
     * @param fifo XXfifoBase的派生类
     */
    void configFifo(const QString &name, XXfifoBase *fifo);

    /**
     * @brief setFifoEnable 设置fifo的输出使能
     * @param name fifo的别名，默认fifo别名有：kdefaultQDebugFifoName和kDefaultFileFifoName
     * @param enable 是否使能
     */
    void setFifoEnable(const QString &name, bool enable);

    /**
     * @brief resetFifo 重置fifo，对应的参数需要与fifo派生类协定
     * @param name fifo的别名，默认fifo别名有：kdefaultQDebugFifoName和kDefaultFileFifoName
     * @param param fifo的参数
     */
    void resetFifo(const QString &name, const QVariantMap &param);

    /**
     * @brief instance 单例模式
     */
    static XXlogger& instance();

public: // 一些公开常量
    static const char * const kDefaultQDebugFifoName;
    static const char * const kDefaultFileFifoName;

private:
    void initDefaultFifo();
    void output(const QString &msg, XXfifoBase *without = nullptr);

private:
    bool _qDebugFifoEnable;  // 由于qDebug使用的qt自带的，并没有进行派生封装，所以这里需要有一个变量描述其是否使能，默认是true；
    QMap<QString,QSharedPointer<XXfifoBase>> _nameToFifo;
    static XXlogger *_instance;
};

#endif // XXLOGGER_H
