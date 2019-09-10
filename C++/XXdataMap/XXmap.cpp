#include "XXmap.h"

XXmapRef::XXmapRef(XXmap &map)
    :_map(map){}
XXmapRef::XXmapRef(const XXmapRef &mapRef)
    : _map(mapRef._map)
    , _path(mapRef._path){}

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

void XXmapRef::operator=(const std::string &value){
    _map._data.insert(_path._data, value);
}
void XXmapRef::operator=(int value){
    _map._data.insert(_path._data, std::to_string(value));
}
void XXmapRef::operator=(double value){
    _map._data.insert(_path._data, std::to_string(value));
}
void XXmapRef::operator=(const std::list<std::string> &list){

}
