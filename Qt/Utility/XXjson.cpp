#include "XXjson.h"
#include <QFile>
#include <QJsonParseError>
#include <QFileInfo>
#include <QDir>
#include "../XXqtDefine.h"

#define logger
#ifdef logger
#include "./XXlogger.h"
#include <QDebug>
#else
#include <QDebug>
#endif

XXjson::XXjson(){
    _path="";
}
XXjson::XXjson(const QJsonObject &jobj){
    _path="";
    _root = XXjsonItem("",jobj,nullptr);
}
XXjson::XXjson(const QString &path){
    _path = path;
    QJsonObject jobj;
    if(QFileInfo(_path).exists()){
        loadFromFile(path,jobj);
    }
    else{
        XX_SAFE_FILEPATH(path,true);
    }
    _root = XXjsonItem("",jobj,nullptr);
}
XXjson::~XXjson(){
}

XXjsonItem XXjson::operator[](const QString &key){
    return _root[key];
}
QJsonObject XXjson::operator()(void){
    return _root._jvalue.toObject();
}

bool XXjson::loadFromFile(const QString &path){
    QJsonObject jobj;
    bool ret = loadFromFile(path,jobj);
    _root = XXjsonItem("",jobj,nullptr);
    return ret;
}
bool XXjson::loadFromFile(const QString &path, QJsonObject &jobj){
    QFile file(path);
    if(!file.exists()){
#ifdef logger
        xxLogStr(QString("file not exist. path:%1").arg(path));
#else
        qDebug() << (QString("file not exist. path:%1").arg(path));
#endif
        return false;
    }
    if(!file.open(QIODevice::ReadOnly)){
#ifdef logger
        xxLogStr(QString("failure to open file. path:%1").arg(path));
#else
        qDebug() << (QString("failure to open file. path:%1").arg(path));
#endif
        return false;
    }

    QJsonParseError error;
    auto jsonDoc = QJsonDocument::fromJson(file.readAll(),&error);
    file.close();
    if(QJsonParseError::NoError != error.error){
#ifdef logger
        xxLogStr(QString("failure to read json. path:%1 error:%2").arg(path).arg(error.errorString()));
#else
        qDebug() << (QString("failure to read json. path:%1 error:%2").arg(path).arg(error.errorString()));
#endif
        return false;
    }

    jobj = jsonDoc.object();
    return true;
}
bool XXjson::saveToFile(const QString &path){
    QString realPath = path.isEmpty()?_path:path;

    QJsonDocument jDoc = QJsonDocument(_root._jvalue.toObject());
    QByteArray bytes = jDoc.toJson();

    QFileInfo fileInfo(realPath);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        QDir().mkpath(dir.absolutePath());
    }

    QFile file(realPath);
    if (file.exists()) {
        file.remove();
    }

    if (!file.open(QIODevice::ReadWrite)) {
#ifdef logger
        xxLogStr(QString("failure to open file. path:%1").arg(realPath));
#else
        qDebug() << (QString("failure to open file. path:%1").arg(realPath));
#endif
        return false;
    }

    file.write(bytes);
    file.close();
    return false;
}



XXjsonItem::XXjsonItem(const QVariant &key, const QJsonValue &jvalue, XXjsonItem *parent)
    : _key(key)
    , _jvalue(jvalue)
    , _parent(parent){
}
XXjsonItem::~XXjsonItem(){
}

XXjsonItem XXjsonItem::operator[](QVariant key){
    if(QJsonValue::Undefined!=_jvalue.type() && QVariant::Int==key.type()){
        auto array = _jvalue.toArray();
        if(key.toInt() > array.count()){
            return XXjsonItem("",QJsonValue());
        }
        else if(key.toInt() == array.count()){
            array.append(QJsonValue());
            _jvalue = array;
        }
        else{

        }

        auto value = array[key.toInt()];
        return XXjsonItem(key,value,this);
    }
    else if(QJsonValue::Undefined!=_jvalue.type() && QVariant::String == key.type()){
        auto object = _jvalue.toObject();
        if (!object.contains(key.toString())) {
            object.insert(key.toString(), QJsonValue());
            _jvalue = object;
        }

        auto value = object[key.toString()];
        return XXjsonItem(key,value,this);
    }
    else {
        return XXjsonItem("",QJsonValue());
    }
}
void XXjsonItem::operator=(QVariant value){
    _jvalue = QJsonValue::fromVariant(value);
    XXjsonItem *item = this;
    while(nullptr != item){
        item->saveAtParent();
        item = item->_parent;
    }
    qDebug()<<"";
}
void XXjsonItem::operator=(const XXjsonItem &item){
    _key = item._key;
    _jvalue = item._jvalue;
    _parent = item._parent;
}

QString XXjsonItem::toString() const{
    return _jvalue.toString();
}
int XXjsonItem::toInt() const {
    return _jvalue.toInt();
}
bool XXjsonItem::containts(QVariant key){
    if(QJsonValue::Array==_jvalue.type() && QVariant::Int==key.type()){
        return _jvalue.toArray().count()>key.toInt() && key.toInt()>=0;
    }
    else if(QJsonValue::Object==_jvalue.type() && QVariant::String==key.type()){
        return _jvalue.toObject().contains(key.toString()) && !key.toString().isEmpty();
    }
    else {
        return false;
    }
}

void XXjsonItem::saveAtParent(){
    if(nullptr == _parent){
        return;
    }
    if(QVariant::Int == _key.type()){
        QJsonArray array = _parent->_jvalue.toArray();
        array[_key.toInt()] = _jvalue;
        _parent->_jvalue = array;
    }
    else if(QVariant::String == _key.type()){
        QJsonObject object = _parent->_jvalue.toObject();
        object[_key.toString()] = _jvalue;
        _parent->_jvalue = object;
    }
    else {

    }
}
