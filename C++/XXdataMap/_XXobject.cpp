#include "XXobject.h"

//
XXintObject::XXintObject(int value)
    : XXobject(XXobjectType_Int){
    _intValue = value;
}
XXobject* XXintObject::create() const{
    return new XXintObject(_intValue);
}
XXintObject::operator int() const{
    return _intValue;
}
void XXintObject::copy(const XXobject &object){
    _intValue = object._intValue;
}

//
XXdoubleObject::XXdoubleObject(double value)
    : XXobject(XXobjectType_Double){
    _doubleValue = value;
}
XXobject* XXdoubleObject::create() const{
    return new XXdoubleObject(_doubleValue);
}
XXdoubleObject::operator double() const{
    return _doubleValue;
}
void XXdoubleObject::copy(const XXobject &object){
    _doubleValue = object._doubleValue;
}

//
XXstringObject::XXstringObject(std::string value)
    : XXobject(XXobjectType_String){
    _stringValue = value;
}
XXobject* XXstringObject::create() const{
    return new XXstringObject(_stringValue);
}
XXstringObject::operator std::string() const{
    return _stringValue;
}
void XXstringObject::copy(const XXobject &object){
    _stringValue = object._stringValue;
}

//
XXvectorObject::XXvectorObject(XXobjectSharedVector objectSharedVector)
    : XXobject(XXobjectType_Vector){
    _vectorValue = objectSharedVector;
}
XXobject* XXvectorObject::create() const{
    return new XXvectorObject(_vectorValue);
}
XXvectorObject::operator XXobjectSharedVector() const{
    return _vectorValue;
}
void XXvectorObject::copy(const XXobject &object){
    _vectorValue = object._vectorValue;
}

//
XXmapObject::XXmapObject(XXobjectSharedMap objectSharedMap)
    : XXobject(XXobjectType_Map){
    _mapValue = objectSharedMap;
}
XXobject* XXmapObject::create() const{
    return new XXmapObject(_mapValue);
}
XXmapObject::operator XXobjectSharedMap() const{
    return _mapValue;
}
void XXmapObject::copy(const XXobject &object){
    _mapValue = object._mapValue;
}