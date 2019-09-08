#ifndef XXdataMap_h
#define XXdataMap_h

#include <list>
#include <string>

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
// XXdMap
//
class XXdMap{
    friend class XXdMapRef;
public:
	XXdMap(const char *name) : _name(name) {}
	XXdMap(const std::string &name) : _name(name) {}
	XXdMap(const XXdMap &dmap) { *this = dmap }
    XXdMap(std::map<std::string,std::string> map) :_map(map){}
	virtual ~XXdMap() {}

    void operator(const XXdMap &dmap){
        _name   = dmap._name;
        _map    = dmap._map;
    }

private:
    std::string _name;
	std::map<std::string,std::string> _map;
};


//
// XXdMapRef
//
class XXdMapRef{
public:
    XXdMapRef(XXdMap &dmap):_dmap(dmap);
    XXdMapRef(XXdMapRef dmapRef);
    virtual ~XXdMapRef(){}

public:
    XXdMapRef operator[](const std::string &key);
    XXdMapRef operator()(uint16_t index);
    XXdMapRef operator()(cosnt std::string &index);
    
public:
    void operator=(const std::string &value);
    void operator=(int value);
    void operator=(double value);
    void operator=(const std::list<std::string> &list);

public:
    std::string operator() const;
    std::string toString() const;
    int toInt() const;
    double toDouble() const;
    std::list<std::string> toList() const;

public:
    void listInsert(unsigned int index, const std::string &value);
    void listRemove(unsigned int index);
    std::string listAt(unsigned int index);

private:
    XXdMap &_dmap;
    bool _isList;
};
#endif