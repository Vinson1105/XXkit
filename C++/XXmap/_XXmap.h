#ifndef XXmap_h
#define XXmap_h

#include <stdio.h>
#include <map>
#include "XXdataBase.h"

//
// XXmap
//
class XXmap : public XXdataBase
{
public:
	XXmap(const char *name) : XXdataBase(name) {}
	XXmap(const std::string &name) : XXdataBase(name) {}
	XXmap(XXmap &xxmap) : XXdataBase(xxmap) {}
	XXmap() : XXdataBase("") {}
	~XXmap() {}
private:
	bool contains(const std::string &key) const {
		return _map.end() != _map.find(key);
	}
	XXobjectShared get(const std::string &key) const {
		auto iter = _map.find(key);
		if (_map.end() == iter){
			return nullptr;
		}
		return iter->second;
	}
	void set(const std::string &key, const XXobject &object) {
		_map[key] = XXobjectShared(new XXobject(object));
	}
	void set(const std::string &key, XXobjectShared objectShared){
		_map[key] = objectShared;
	}
	void del(const std::string &key) {
		auto iter = _map.find(key);
		if (_map.end() == iter) {
			return;
		}
		_map.erase(iter);
	}
private:
	std::map<std::string,XXobjectShared> _map;
};

#endif /* XXmap_h */
