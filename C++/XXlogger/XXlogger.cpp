#include "XXlogger.h"
#include <ctime>
#include "FileLogFifo.h"
#include <iostream>
#ifdef I_OS_WIN
#include <windows.h>
#endif

#define ASPRINTF(format,...) (QString::asprintf(format,##__VA_ARGS__))

XXlogger* XXlogger::_instance = nullptr;
const char* const XXlogger::kDefaultPrintfFifoName = "qDebugFifo";
const char* const XXlogger::kDefaultFileFifoName = "fileFifo";

XXlogger::XXlogger()
    : _printfFifoEnable(true){
    initDefaultFifo();
}

void XXlogger::message(const std::string &msg){
    this->output(msg.data());
}
void XXlogger::message(XXfifoBase *without, const std::string &msg){
    this->output(msg.data(),without);
}
void XXlogger::configFifo(const std::string &name, XXfifoBase *fifo){
    _nameToFifo[name] = std::shared_ptr<XXfifoBase>(fifo);
    fifo->setEnable(true);
    fifo->setName(name);
}
void XXlogger::setFifoEnable(const std::string &name, bool enable){
    if(name == kDefaultPrintfFifoName){
        _printfFifoEnable = enable;
        return;
    }
	auto iter = _nameToFifo.find(name);
    if(_nameToFifo.end() == iter){
        return;
    }
	iter->second->setEnable(enable);
}
void XXlogger::resetFifo(const std::string &name, const Json::Value &param){
    if(name == kDefaultPrintfFifoName){
        return;
    }
	auto iter = _nameToFifo.find(name);
    if(_nameToFifo.end() == iter){
        return;
    }
	iter->second->reset(param);
}

XXlogger& XXlogger::instance(){
    if(nullptr == _instance){
        _instance = new XXlogger;
    }
    return *_instance;
}

void XXlogger::initDefaultFifo(){
    // 默认的文件fifo配置
	std::string path = this->homePath("aaaa");
    FileLogFifo *fifo = new FileLogFifo({
                                            {FileLogFifo::kLogDir,path}
                                        });

    configFifo(kDefaultFileFifoName,fifo);
}
void XXlogger::output(const std::string &msg, XXfifoBase *without){
	std::string msgWithDateTime;
	msgWithDateTime.append("[");
	msgWithDateTime.append(currentDateTime());
	msgWithDateTime.append("] ");
	msgWithDateTime.append(msg);

    const char *bytes = msgWithDateTime.data();
    for (auto iter=_nameToFifo.cbegin(); iter!=_nameToFifo.cend(); iter++) {
        if(!iter->second->isEnabled() || without == iter->second.get()){
            continue;
        }
        iter->second->push(bytes);
    }
    if(_printfFifoEnable){
		std::cout << msg << std::endl;
    }
}
std::string XXlogger::homePath(const std::string &fileName) {
	std::string path = ".";
#ifdef I_OS_WIN
	const char* homeProfile = "USERPROFILE";
	char homePath[1024] = { 0 };
	unsigned int pathSize = GetEnvironmentVariable(homeProfile, homePath, 1024);

	if (pathSize == 0 || pathSize > 1024) {
		int ret = GetLastError();
	}
	else {
		path = std::string(homePath);
		if (fileName.length() > 0) {
			path += "\\" + fileName;
		}
	}
#endif
	return path;
}
const char* XXlogger::currentDateTime() {
	static char buffer[1024] = { 0 };
	memset(buffer, 0, 1024);

#ifdef I_OS_WIN
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);  //获取当前时间 可精确到ms
	sprintf_s(buffer, "%d-%02d-%02d %02d:%02d:%02d.%03d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);
#else
	time_t now = time(0);
	tm *ltm = localtime(&now);

	sprintf_s(buffer,"%d-%02d-%02d %02d:%02d:%02d",
		(1900+ltm->tm_year),
		(1+ ltm->tm_mon),
		(ltm->tm_mday),
		(ltm->tm_hour),
		(ltm->tm_min),
		(ltm->tm_sec));
#endif

	return buffer;
}

