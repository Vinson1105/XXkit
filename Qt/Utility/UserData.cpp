#include "UserData.h"
#include <mutex>
#include <QCoreApplication>
#include <QDir>

UserData* UserData::_instance = nullptr;

void UserData::reset(const QString &config, const QString &setting){
    instance()->reset_(config,setting);
}
XXjson& UserData::config(){
    return instance()->config_();
}
XXjson& UserData::setting(){
    return instance()->setting_();
}

UserData *UserData::instance(){
    static std::once_flag once;
    std::call_once(once ,[&](void){
        _instance = new UserData;
    });
    return _instance;
}
UserData::UserData(QObject *parent)
    : QObject(parent){
    _config.reset(new XXjson(QCoreApplication::applicationDirPath()+"/config/main"));
    _setting.reset(new XXjson(QDir::homePath()+"/"+QCoreApplication::applicationName()+"/setting/main"));
}

void UserData::reset_(const QString &config, const QString &setting){
    if(!config.isEmpty()){
        _config.reset(new XXjson(config));
    }
    if(!setting.isEmpty()){
        _setting.reset(new XXjson(setting));
    }
}
XXjson& UserData::config_(){
    return *_config;
}
XXjson& UserData::setting_(){
    return *_setting;
}
