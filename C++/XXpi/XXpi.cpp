#include "XXpi.h"
#include "../Common/XXstdStringExtend.h"

#define DATASTRING_LENGTH   512
#define BUFFER_LENGTH       4*1024

#define FIRST_KEY      "XXenterPi" 
#define SPLIT_KV        ":"
#define SPLIT_PAIRS     "\r\n"

using namespace std;

XXpi::XXpi(const string &subPi)
: _subPi(subPi){
    _buffer = (char*)malloc(BUFFER_LENGTH);
}
XXpi::~XXpi(){
    free(_buffer);
    _buffer = nullptr;
}

shared_ptr<string> XXpi::getData(){
    shared_ptr<string> stringPtr(new string());
    stringPtr->reserve(DATASTRING_LENGTH);
    addPairToString(stringPtr, FIRST_KEY, _subPi);
    for (auto iter = _map.begin(); iter != _map.end(); iter++)
    {
        addPairToString(stringPtr, iter->first, iter->second);
    }
    return stringPtr;
}
bool XXpi::parseData(const int8_t *data, int length, map<string,string> &map){
    StringShared dataString(new std::string((char*)data, length));
    auto pairs = XXstdStringExtend::split(*dataString, SPLIT_PAIRS);
    for (auto iter = pairs.begin(); iter != pairs.end(); iter++)
    {
        auto key = XXstdStringExtend::section(**iter, SPLIT_KV, 0, 0);
        auto value = XXstdStringExtend::section(**iter, SPLIT_KV, 1, 1);
        map[key] = value;
    }
    return true;
}
int XXpi::composeData(int8_t *data){
    
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