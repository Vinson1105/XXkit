#ifndef FILELOGFIFO_H
#define FILELOGFIFO_H

#include "XXlogger.h"
#include "../third/json/json.h"
#include <iostream>
#include <fstream>
#include <thread>

#include <queue>

class FileLogFifo : public XXfifoBase{
public:
    FileLogFifo(const Json::Value &param);
    ~FileLogFifo();

    void reset(const Json::Value &param);
    void push(const char *data);

public: // 一些公开常量
    static const char * const kLogDir;

private:
    /**
     * @brief openFile 根据目录和文件名称组成路径打开文件
     * @param dirPath 文件对应的目录
     * @param fileName 文件名称
     * @return 打开失败将返回nullptr
     */
    FILE* openFile(const std::string &dirPath, const std::string &fileName);

    /**
     * 由于push等函数可能存在多线程调用的情况，那么在多线程下直接写文件可能会导致一些异常情况；
     * 这里就采取了信号-槽处理，槽直接放到固定线程中，使用事件循环做一个写触发队列；
     */
    Q_SIGNAL void sigPush(QSharedPointer<QByteArray> bytes);
    Q_SIGNAL void sigReset(QVariantMap param);

    Q_SLOT void onPush(QSharedPointer<QByteArray> bytes);
    Q_SLOT void onReset(QVariantMap param);
private:
	std::ofstream  *_file;   // 输出文件的
    std::thread *_thread;   // 写文件线程
	std::queue 
};

#endif // FILELOGFIFO_H
