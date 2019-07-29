#include "XXmap.h"

XXmapRef::XXmapRef(XXmapBase &mapBase) 
	: _mapBase(mapBase)
{}
XXmapRef::XXmapRef(const XXmapRef &mapRef)
	: _mapBase(mapRef._mapBase)
	, _path(mapRef._path)
{}
XXmapRef::~XXmapRef() 
{}

void XXmapRef::operator=(const XXmapRef &mapRef) {

}
void XXmapRef::operator=(const string &value) {
	_mapBase.set(_path.path(), value);
}
void XXmapRef::operator=(int value) {
	_mapBase.set(_path.path(), value);
}

XXmapRef XXmapRef::operator[](const string &key) {
	XXmapRef mapRef(_mapBase);
	mapRef._path = _path;
	mapRef._path << key;
	return mapRef;
}
XXmapRef XXmapRef::operator[](int key) {
	XXmapRef mapRef(_mapBase);
	mapRef._path = _path;
	mapRef._path << key;
	return mapRef;
}

string XXmapRef::toString() const {
	return _mapBase.get(_path.path());
}
int XXmapRef::toInt() const {
	return atoi(_mapBase.get(_path.path()).c_str());
}