#ifndef XXmap_h
#define XXmap_h

#include <list>
#include <string>
#include <map>

#define XXMAP_VALUE_INVALID ""  // 无效值

//
// XXpath
//
class XXpath {
    friend class XXmapRef;
public:
	XXpath(){}
	XXpath(const char *path) : _data(path){}
	XXpath(const std::string &path) : _data(path){}
	XXpath(const XXpath &xxpath) {*this = xxpath;}
	virtual ~XXpath(){}

	const std::string& path() const{ return _data; }

	void operator=(const XXpath &xxpath)        { _data = xxpath._data;}
	void operator<<(const char *node)           { _data += '/'; _data += node;}
	void operator<<(const std::string &node)    { _data += '/'; _data += node;}
	void operator<<(int node)                   { _data += '/'; _data += std::to_string(node);}

private:
    std::string _data;
};

//
// XXmap
//
class XXmap{
    friend class XXmapRef;
public:
	XXmap(const char *name) : _name(name) {}
	XXmap(const std::string &name) : _name(name) {}
	XXmap(const XXmap &xxmap) { *this = xxmap; }
    XXmap(std::map< std::string,std::string > map) :_data(map){}
	virtual ~XXmap() {}

    void operator=(const XXmap &xxmap){
        _name   = xxmap._name;
        _data   = xxmap._data;
    }

private:
    std::string _name;
	std::map<std::string,std::string> _data;
};


//
// XXmapRef
//
class XXmapRef{
public:
    XXmapRef(XXmap &map);
    XXmapRef(const XXmapRef &mapRef);
    virtual ~XXmapRef(){}

public:
    XXmapRef operator[](const std::string &key);
    XXmapRef operator()(unsigned int index);
    XXmapRef operator()(const std::string &index);
    
public:
    void operator=(const std::string &value);
    void operator=(int value);
    void operator=(double value);
    void operator=(const std::list<std::string> &list);

public:
    operator std::string() const;
    std::string toString() const;
    int toInt() const;
    double toDouble() const;
    std::list<std::string> toList() const;

public:
    void listInsert(unsigned int index, const std::string &value);
    void listRemove(unsigned int index);
    std::string listAt(unsigned int index);

private:
    XXpath _path;
    XXmap &_map;
    bool _isList;
};
#endif