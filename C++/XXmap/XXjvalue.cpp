#include "XXjvalue.h"
#include "../Common/XXstdStringExtend.h"

XXjvalue XXjvalue::create(int deepness, Type type, const std::string &value){
    return XXjvalue(deepness, type, value);
}
XXjvalue XXjvalue::create(const std::string &jvalueString){
    return XXjvalue(jvalueString);
}

XXjvalue::XXjvalue(int deepness, Type type, const std::string &value)
    : _deepness(deepness)
    , _type(type)
    , _value(value)
    {}
XXjvalue::XXjvalue(const std::string &jvalueString){
    fromData(jvalueString);
}

std::string XXjvalue::toData() const{
    std::string jvalueString = std::to_string(_deepness);
    jvalueString += "/" + std::to_string(_type);
    jvalueString += "/" + _value;
    return jvalueString;
}
XXjvalue& XXjvalue::fromData(const std::string &string){
    const char *cData = string.c_str();
   if ('/' == cData[1] && '/' == cData[3]){
       _deepness    = std::stoi(string.substr(0,1));
       _type        = (Type)std::stoi(string.substr(2,1));
       _value       = std::string(string.c_str()+4);
    }
    return *this;
}
void XXjvalue::operator=(const XXjvalue &jvalue){
    _deepness   = jvalue.deepness();
    _type       = jvalue.type();
    _value      = jvalue.value();
}
