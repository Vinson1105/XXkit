#include "XXmap.h"

#define VALUE_LIST_PREFIX "()"      // 数组中间节点的标识，如：1/2/3 : (),COUNT,0,1,2
#define VALUE_LIST_DELIMITER ','    // 数组节点/值的分隔符    

/// 构建析构函数
XXmapRef::XXmapRef(XXmap &map)
    :_map(map){}
XXmapRef::XXmapRef(const XXmapRef &mapRef)
    : _map(mapRef._map)
    , _path(mapRef._path){}

/// 公共函数 - 路径编辑
XXmapRef XXmapRef::operator[](const std::string &key){
    XXmapRef mapRef(*this);
    mapRef._path << key;
    return mapRef;
}
XXmapRef XXmapRef::operator()(unsigned int index){
    XXmapRef mapRef(*this);
    mapRef._path << std::to_string(index);
    return mapRef;
}
XXmapRef XXmapRef::operator()(const std::string &index){
    XXmapRef mapRef(*this);
    mapRef._path << index;
    return mapRef;
}

/// 公共函数 - 赋值函数
void XXmapRef::operator=(const std::string &value){
    if(XXMAP_VALUE_INVALID == value)
        return;
    _map._data.insert(_path._data, value);
}
void XXmapRef::operator=(int value){
    _map._data.insert(_path._data, std::to_string(value));
}
void XXmapRef::operator=(double value){
    _map._data.insert(_path._data, std::to_string(value));
}
void XXmapRef::operator=(const std::list<std::string> &list){
    if (0 == list.size())
        return;
    
    std::string listString;
    for (auto iter = list.cbegin(); iter != list.cend(); iter++){
        listString += *iter;
        if (iter + 1 != list.end()){        
            listString += ',';
        }
    }

    _map._data.insert(_path._data, listString);
}

/// 公共函数 - 取值函数
XXmapRef::operator std::string() const{
    return this->toString();
}
std::string XXmapRef::toString() const{
    auto iter = _map._data.find(_path._data);
    if (_map._data.end() == iter){
        return XXMAP_VALUE_INVALID;
    }
    return iter->second;
}
int XXmapRef::toInt() const{
    return std::stoi(this->toString());
}
double XXmapRef::toDouble() const{
    return std::stod(this->toString());
}
std::list<std::string> XXmapRef::toList() const{
    std::list<std::string> stringList;
    std::string value = this->toString();
    
}