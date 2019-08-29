#include "XXpi.h"

#define FIRST_KEY      "XXenterPi" 
#define SPLIT_KV        ":"
#define SPLIT_PAIRS     "\r\n"

XXpi::XXpi(const string &subPi)
    : _subPi(subPi){
}
XXpi::~XXpi(){}

const string& XXpi::getData(){
    string data = FIRST_KEY;
    data += SPLIT_KV; data += _subPi; data += SPLIT_PAIRS;
    for (map<string,string>::iterator iter = _map.begin(); iter != _map.end(); iter++)
    {
        data += 
    }
    
}
bool XXpi::parseData(const int8_t *data, int length, map<string,string> &map){
    
}