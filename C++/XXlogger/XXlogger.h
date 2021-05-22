#ifndef XXLOGGER_H
#define XXLOGGER_H

#include <string>
#include <memory>
#include "../../XXdefine.h"
#include "../third/json/json.h"

#ifdef I_OS_WIN
    #define xxLog(format,...) \
    {\
        QString msg = QString("[%1] [%2] %3\n").arg(__FUNCTION__).arg(__LINE__).arg(QString::asprintf(format,##__VA_ARGS__)); \
        XXlogger::instance().message(msg); \
    }
#else
#define xxLog(format,args...) \
    {\
        QString msg = QString("[%1] [%2] %3\n").arg(__FUNCTION__).arg(__LINE__).arg(QString::asprintf(format,##args)); \
        XXlogger::instance().message(msg); \
    }
#endif

#define xxLogStr(string)  \
    {\
        QString msg = QString("[%1] [%2] %3\n").arg(__FUNCTION__).arg(__LINE__).arg(string); \
        XXlogger::instance().message(msg); \
    }

#ifdef I_OS_WIN
    #define xxLogWithout(fifo,format,...) \
    {\
        QString msg = QString("[%1] [%2] %3\n").arg(__FUNCTION__).arg(__LINE__).arg(QString::asprintf(format,##__VA_ARGS__)); \
        XXlogger::instance().message(fifo,msg); \
    }
#else
#define xxLogWithout(fifo,format,args...) \
    {\
        QString msg = QString("[%1] [%2] %3\n").arg(__FUNCTION__).arg(__LINE__).arg(QString::asprintf(format,##args)); \
        XXlogger::instance().message(fifo,msg); \
    }
#endif

#define xxLogStrWithout(fifo,string)  \
    {\
        QString msg = QString("[%1] [%2] %3\n").arg(__FUNCTION__).arg(__LINE__).arg(string); \
        XXlogger::instance().message(fifo,msg); \
    }

class XXfifoBase{
public:
    XXfifoBase(){}
    virtual ~XXfifoBase() {}

    virtual void reset(const Json::Value &param) = 0;
    virtual void push(const char *data) = 0;
    virtual bool isEnabled() const { return _enable; }
    virtual void setEnable(bool enable){ _enable = enable; }
    virtual std::string name() const { return _name; }
    virtual void setName(const std::string &name) { _name = name; }

protected:
    bool _enable;
    std::string _name;
};

/**
 * @brief The XXlogger class
 * 默认配置了以下fifo
 *  - printf，name=“printfFifo”
 *  - file，name=“fileFifo”，默认路径为/「home」/「App」/xxlogger，文件格式为「yyyy-MM-dd」
 */
class XXlogger {
public:
    XXlogger();
    virtual ~XXlogger() {}

    /**
     * @brief message 信息输出
     * @param msg 需要输出的信息
     */
    void message(const std::string &msg);

    /**
     * @brief message 信息输出，并可以过滤指定的fifo
     * @param msg 需要输出的信息
     */
    void message(XXfifoBase *without, const std::string &msg);

    /**
     * @brief configFifo 配置目标fifo（需要继承XXfifoBase），并移交释放权到XXlogger中，name相当是对应fifo的控制key；
     * @param name fifo的别名，不要与默认别名重复，默认fifo别名有：kdefaultQDebugFifoName和kDefaultFileFifoName
     * @param fifo XXfifoBase的派生类
     */
    void configFifo(const std::string &name, XXfifoBase *fifo);

    /**
     * @brief setFifoEnable 设置fifo的输出使能
     * @param name fifo的别名，默认fifo别名有：kdefaultQDebugFifoName和kDefaultFileFifoName
     * @param enable 是否使能
     */
    void setFifoEnable(const std::string &name, bool enable);

    /**
     * @brief resetFifo 重置fifo，对应的参数需要与fifo派生类协定
     * @param name fifo的别名，默认fifo别名有：kdefaultQDebugFifoName和kDefaultFileFifoName
     * @param param fifo的参数
     */
    void resetFifo(const std::string &name, const Json::Value &param);

    /**
     * @brief instance 单例模式
     */
    static XXlogger& instance();

public: // 一些公开常量
    static const char * const kDefaultPrintfFifoName;
    static const char * const kDefaultFileFifoName;

private:
    void initDefaultFifo();
    void output(const std::string &msg, XXfifoBase *without = nullptr);
	std::string homePath(const std::string &fileName = "");
	const char* currentDateTime();

private:
    bool _printfFifoEnable;  // 由于qDebug使用的qt自带的，并没有进行派生封装，所以这里需要有一个变量描述其是否使能，默认是true；
    std::map<std::string,std::shared_ptr<XXfifoBase>> _nameToFifo;
    static XXlogger *_instance;
};

#endif // XXLOGGER_H
