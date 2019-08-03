#include "XXobject.h"

XXintObject::XXintObject(int value)
    : XXobject(XXobjectType_Int){
    _intValue = value;
}
XXintObject::operator int() const{
    return _intValue;
}
void XXintObject::copy(const XXobject &object){
    _intValue = object._intValue;
}

XXdoubleObject::XXdoubleObject(double value)
    : XXobject(XXobjectType_Double){
    _doubleValue = value;
}
XXdoubleObject::operator double() const{
    return _doubleValue;
}
void XXdoubleObject::copy(const XXobject &object){
    _doubleValue = object._doubleValue;
}

XXstringObject::XXstringObject(std::string value)
    : XXobject(XXobjectType_String){
    _stringValue = value;
}
XXstringObject::operator std::string() const{
    return _stringValue;
}
void XXstringObject::copy(const XXobject &object){
    _stringValue = object._stringValue;
}