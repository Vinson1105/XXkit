#include "XXmap.h"

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
void XXdataBaseRef::operator=(XXobjectShared objectShared){
	_dataBase.set(_path.path(), objectShared);
}
void XXdataBaseRef::operator=(int value) {
	_dataBase.set(_path.path(), XXobjectShared(new XXintObject(value)));
}
void XXdataBaseRef::operator=(double value) {
	_dataBase.set(_path.path(), XXobjectShared(new XXdoubleObject(value)));
}
void XXdataBaseRef::operator=(const string &value) {
	_dataBase.set(_path.path(), XXobjectShared(new XXstringObject(value)));
}

XXobjectShared XXdataBaseRef::object(){
	return _dataBase.get(_path.path());
}
XXdataBaseRef::operator XXobjectShared() const{
	return _dataBase.get(_path.path());
}
// XXdataBaseRef::operator int() const{
// 	XXobjectShared objectShared = _dataBase.get(_path.path());
// 	XXobject *object 			= objectShared.get();
// 	if (objectShared->isInt()){
// 		XXintObject *intObject = dynamic_cast<XXintObject*>(object);
// 		return *intObject;
// 	}
// 	return 0;
// }
// XXdataBaseRef::operator double() const{
// 	XXobjectShared objectShared = _dataBase.get(_path.path());
// 	XXobject *object 			= objectShared.get();
// 	if (objectShared->isDouble()){
// 		XXdoubleObject *doubleObject = dynamic_cast<XXdoubleObject*>(object);
// 		return *doubleObject;
// 	}
// 	return 0.0;
// }
// XXdataBaseRef::operator std::string() const{
// 		XXobjectShared objectShared = _dataBase.get(_path.path());
// 	XXobject *object 			= objectShared.get();
// 	if (objectShared->isDouble()){
// 		XXstringObject *stringObject = dynamic_cast<XXstringObject*>(object);
// 		return *stringObject;
// 	}
// 	return "";
// }

XXdataBaseRef XXdataBaseRef::operator[](const string &key) {
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