#include "XXdataBase.h"

XXdataBaseRef::XXdataBaseRef(XXdataBase &dataBase) 
	: _dataBase(dataBase)
{}
XXdataBaseRef::XXdataBaseRef(const XXdataBaseRef &dataBaseRef)
	: _dataBase(dataBaseRef._dataBase)
	, _path(dataBaseRef._path)
{}
XXdataBaseRef::~XXdataBaseRef() 
{}

void XXdataBaseRef::operator=(const XXdataBaseRef &dataBaseRef) {
	_dataBase.set(_path.path(), dataBaseRef);
}
void XXdataBaseRef::operator=(const XXobject &object){
	XXobjectShared objectShared(object.create());
	_dataBase.set(_path.path(), objectShared);
}
void XXdataBaseRef::operator=(XXobjectShared objectShared){
	_dataBase.set(_path.path(), objectShared);
}
void XXdataBaseRef::operator=(int value) {
	_dataBase.set(_path.path(), XXobjectShared(new XXintObject(value)));
}
void XXdataBaseRef::operator=(double value) {
	_dataBase.set(_path.path(), XXobjectShared(new XXdoubleObject(value)));
}
void XXdataBaseRef::operator=(const std::string &value) {
	_dataBase.set(_path.path(), XXobjectShared(new XXstringObject(value)));
}
void XXdataBaseRef::operator=(const XXobjectSharedVector &objectSharedVector){
    _dataBase.set(_path.path(), XXobjectShared(new XXvectorObject(objectSharedVector)));
}
void XXdataBaseRef::operator=(const XXobjectSharedMap &objectSharedMap){
	_dataBase.set(_path.path(), XXobjectShared(new XXmapObject(objectSharedMap)));
}

XXobjectShared XXdataBaseRef::obj(){
	return _dataBase.get(_path.path());
}
XXdataBaseRef::operator XXobjectShared() const{
	return _dataBase.get(_path.path());
}

XXdataBaseRef XXdataBaseRef::ref(){
    XXdataBaseRef dataBaseRef(_dataBase);
    return dataBaseRef;
}
XXdataBaseRef XXdataBaseRef::operator[](const std::string &key) {
	XXdataBaseRef dataBaseRef(_dataBase);
	dataBaseRef._path = _path;
	dataBaseRef._path << key;
	return dataBaseRef;
}
XXdataBaseRef XXdataBaseRef::operator[](int key) {
	XXdataBaseRef dataBaseRef(_dataBase);
	dataBaseRef._path = _path;
	dataBaseRef._path << key;
	return dataBaseRef;
}