#ifndef XXdataBase_h
#define XXdataBase_h

#include "XXobject.h"
#include <memory>


//
// XXpath
//
class XXpath {
public:
	XXpath(){}
	XXpath(const char *path) : _path(path){}
	XXpath(const std::string &path) : _path(path){}
	XXpath(const XXpath &xxpath) {*this = xxpath;}
	virtual ~XXpath(){}

	const std::string& path() const{ return _path; }

	void operator=(const XXpath &xxpath)        { _path = xxpath._path;}
	XXpath& operator<<(const char *node)        { _path += '/'; _path += node; return *this; }
	XXpath& operator<<(const std::string &node) { _path += '/'; _path += node; return *this; }
	XXpath& operator<<(int node)                { _path += '/'; _path += std::to_string(node); return *this; }

private:
    std::string _path;
};

//
// XXdataBase
//
class XXdataBase {
	friend class XXdataBaseRef;
protected:
	XXdataBase(const char *name) : _name(name) {}
	XXdataBase(const std::string &name) : _name(name) {}
	XXdataBase(XXdataBase &mapBase) : _name("") {}
	XXdataBase() : _name("") {}
public:
	virtual ~XXdataBase() {}

protected:
	virtual bool contains(const std::string &key) const = 0;
	virtual XXobjectShared get(const std::string &key) const = 0;
	virtual void set(const std::string &key, const XXobject &object) = 0;
	virtual void set(const std::string &key, XXobjectShared objectShared) = 0;
	virtual void del(const std::string &key) = 0;

private:
	std::string _name;
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
	void operator=(const XXobject &object);
	void operator=(XXobjectShared objectShared);
	void operator=(int value);
	void operator=(double value);
	void operator=(const std::string &value);
    void operator=(const XXobjectSharedVector &objectSharedVector);
	void operator=(const XXobjectSharedMap &objectSharedMap);

	XXobjectShared obj();
	operator XXobjectShared() const; 
    
    XXdataBaseRef ref();
	XXdataBaseRef operator[](const std::string &key);
	XXdataBaseRef operator[](int key);
	// XXdataBaseRef operator()(const std::string &key);
	// XXdataBaseRef operator()(int key);

private:
	const XXdataBase& dataBase() const { return _dataBase; }

private:
	XXdataBase &_dataBase;
	XXpath _path;
};

#endif