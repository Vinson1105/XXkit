#ifndef XXmap_h
#define XXmap_h

#include <stdio.h>
#include <map>
#include "XXpath.h"
#include "XXobject.h"
#include <memory>

typedef std::shared_ptr<XXobject> XXobjectShared;

//
// XXdataBase
//
class XXdataBase {
	friend class XXdataBaseRef;
protected:
	XXdataBase(const char *name) : _name(name) {}
	XXdataBase(const string &name) : _name(name) {}
	XXdataBase(XXdataBase &mapBase) : _name("") {}
	XXdataBase() : _name("") {}
public:
	virtual ~XXdataBase() {}

protected:
	virtual bool contains(const string &key) const = 0;
	virtual XXobjectShared get(const string &key) const = 0;
	virtual void set(const string &key, const XXobject &object) = 0;
	virtual void set(const string &key, XXobjectShared objectShared) = 0;
	virtual void del(const string &key) = 0;

private:
	string _name;
};

//
// XXmap
//
class XXmap : public XXdataBase
{
public:
	XXmap(const char *name) : XXdataBase(name) {}
	XXmap(const string &name) : XXdataBase(name) {}
	XXmap(XXmap &xxmap) : XXdataBase(xxmap) {}
	XXmap() : XXdataBase("") {}
	~XXmap() {}
private:
	bool contains(const string &key) const {
		return _map.end() != _map.find(key);
	}
	XXobjectShared get(const string &key) const {
		auto iter = _map.find(key);
		if (_map.end() == iter){
			return nullptr;
		}
		return iter->second;
	}
	void set(const string &key, const XXobject &object) {
		_map[key] = XXobjectShared(new XXobject(object));
	}
	virtual void set(const string &key, XXobjectShared objectShared){
		_map[key] = objectShared;
	}
	void del(const string &key) {
		auto iter = _map.find(key);
		if (_map.end() == iter) {
			return;
		}
		_map.erase(iter);
	}
private:
	std::map<std::string,XXobjectShared> _map;
};

//
// XXdataBaseRef
//
class XXdataBaseRef {
public:
	XXdataBaseRef(XXdataBase &dataBase);
	XXdataBaseRef(const XXdataBaseRef &dataBaseRef);	// 在构造函数中，会复制path和引用map
	virtual ~XXdataBaseRef();

	void operator=(const XXdataBaseRef &dataBaseRef);	// 在赋值函数中，只会对应对其进行值复制
	void operator=(XXobjectShared objectShared);
	void operator=(int value);
	void operator=(double value);
	void operator=(const string &value);

	XXobjectShared object();
	operator XXobjectShared() const;
	// operator int() const;
	// operator double() const;
	// operator std::string() const; 

	XXdataBaseRef operator[](const string &key);
	XXdataBaseRef operator[](int key);

private:
	const XXdataBase& dataBase() const { return _dataBase; }

private:
	XXdataBase &_dataBase;
	XXpath _path;
};

#endif /* XXmap_h */
