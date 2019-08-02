#ifndef XXmap_h
#define XXmap_h

#include <stdio.h>
#include <map>
#include "XXpath.h"

class XXmapBase {
	friend class XXmapRef;
public:
	XXmapBase(const char *name) : _name(name) {}
	XXmapBase(const string &name) : _name(name) {}
	XXmapBase(XXmapBase &mapBase) : _name("") {}
	XXmapBase() : _name("") {}
	virtual ~XXmapBase() {}

protected:
	virtual bool contains(const string &key) const = 0;
	virtual string get(const string &key) const = 0;
	virtual void set(const string &key, const string &value) = 0;
	virtual void set(const string &key, int value) = 0;
	virtual void del(const string &key) = 0;

// 	virtual bool contains(const string &key) const { return false; }
// 	virtual string get(const string &key) const { return ""; }
// 	virtual void set(const string &key, const string &value) { return; }
// 	virtual void del(const string &key) { return; }
// 	virtual XXmapBase map(const string &path) { return XXmapBase(); }

private:
	string _name;
};

class XXmap : public XXmapBase
{
public:
	XXmap(const char *name) : XXmapBase(name) {}
	XXmap(const string &name) : XXmapBase(name) {}
	XXmap(XXmap &mapBase) : XXmapBase(mapBase) {}
	XXmap() : XXmapBase() {}
	~XXmap() {}
private:
	bool contains(const string &key) const {
		return _map.end() != _map.find(key);
	}
	string get(const string &key) const {
		auto iter = _map.find(key);
		if (_map.end() == iter){
			return "";
		}
		return iter->second;
	}
	void set(const string &key, const string &value) {
		_map[key] = value;
	}
	void set(const string &key, int value) {
		_map[key] = to_string(value);
	}
	void del(const string &key) {
		auto iter = _map.find(key);
		if (_map.end() == iter) {
			return;
		}
		_map.erase(iter);
	}
private:
	std::map<string,string> _map;
};

class XXmapRef {
public:
	XXmapRef(XXmapBase &mapBase);
	XXmapRef(const XXmapRef &mapRef);			// 在构造函数中，会复制path和引用map
	virtual ~XXmapRef();

	void operator=(const XXmapRef &mapRef);		// 在赋值函数中，只会对应对其进行值复制
	void operator=(const string &value);
	void operator=(int value);

	XXmapRef operator[](const string &key);
	XXmapRef operator[](int key);

	string toString() const;
	int toInt() const;

private:
	const XXmapBase& map() const { return _mapBase; }

private:
	XXmapBase &_mapBase;
	XXpath _path;
};

#endif /* XXmap_h */
