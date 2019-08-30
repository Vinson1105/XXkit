#include "XXpi.h"

#define DEFAULT_DATASTRING_LENGTH 512

#define FIRST_KEY      "XXenterPi" 
#define SPLIT_KV        ":"
#define SPLIT_PAIRS     "\r\n"

XXpi::XXpi(const string &subPi)
    : _subPi(subPi){
}
XXpi::~XXpi(){}

shared_ptr<string> XXpi::getData(){
    shared_ptr<string> stringPtr(new string());
    stringPtr->reserve(DEFAULT_DATASTRING_LENGTH);
    addPairToString(stringPtr, FIRST_KEY, _subPi);
    for (auto iter = _map.begin(); iter != _map.end(); iter++)
    {
        addPairToString(stringPtr, iter->first, iter->second);
    }
    return stringPtr;
}
bool XXpi::parseData(const int8_t *data, int length, map<string,string> &map){
    
}

void XXpi::resetPairs(){
    _map.clear();
}
XXpi& XXpi::addPairToLocalMap(const string &key, const string &value){
    _map[key] = value;
    return *this;
}
XXpi& XXpi::removePairAtLocalMap(const string &key){
    auto iter = _map.find(key);
    if (_map.end() != iter){
        _map.erase(iter);
    }
    return *this;
}

void XXpi::XXpi::addPairToString(shared_ptr<string> &stringPtr, const string &key, const string &value){
    stringPtr->append(key);
    stringPtr->append(SPLIT_KV);
    stringPtr->append(value);
    stringPtr->append(SPLIT_PAIRS);
}