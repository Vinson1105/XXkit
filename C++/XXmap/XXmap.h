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

    operator std::string()                      {return _data;}
	void operator=(const XXpath &xxpath)        { _data = xxpath._data;}
	void operator<<(const char *node)           { 0 == _data.size() ? _data = node : _data += "/"+std::string(node);}
	void operator<<(const std::string &node)    { 0 == _data.size() ? _data = node : _data += "/"+node;}
	void operator<<(int node)                   { 0 == _data.size() ? _data = node : _data += "/"+std::to_string(node);}

private:
    std::string _data;
};

//
// XXmap
//
class XXmap{
    friend class XXmapRef;
public:
    XXmap(){}
	XXmap(const XXmap &xxmap) { *this = xxmap; }
    XXmap(std::map< std::string,std::string > map) :_data(map){}
	virtual ~XXmap() {}

    void operator=(const XXmap &xxmap){ _data = xxmap._data; }
    std::map<std::string,std::string> data() { return _data; }

private:
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

public: /* 路径编辑运算符 */
    XXmapRef operator[](const std::string &key);    /** 一般路径编辑 */
    XXmapRef operator()(unsigned int index);        /** 数组路径编辑 */
    
public: /* 赋值运算符 */
    void operator=(XXmapRef &mapRef);               /** 只会进行值复制，不会产生map的引用 */
    void operator=(const std::string &value);       
    void operator=(int value);
    void operator=(double value);
    void operator=(const std::vector<std::string> &value);  /** 会将数组内容转换成item1,item2,item3,...,itemN */

public: /* 值转换 */
    operator std::string(); /** 省缺类型转换 */
    std::string toString(); /** 路径有误时，或者没有对应的值时，返回XXMAP_VALUE_INVALID */
    int toInt();            /** 路径有误时，或者没有对应的值时，返回0 */
    double toDouble();      /** 路径有误时，或者没有对应的值时，返回0 */
    std::vector<std::string> toVector();    /** 路径有误时，或者没有对应的值时，空的列表 */

public:
    bool isArray();
    unsigned int arrayCount();
    bool arrayTake(unsigned int from, unsigned int to);
    bool arraySwap(unsigned int index1, unsigned int index2);
    bool arrayInsert(unsigned int index);
    bool arrayDelete(unsigned int index);

private: /* 值设置[内部]*/
    void setValue(const std::string &path, const std::vector<std::string> &value);
    void checkAndSetValue(const std::string &path, const std::string &value);
    std::string checkAndGetValue(const std::string &path);
    bool getRealPath(const std::string &path, std::string &realPath, bool creating);

private: /* 数组信息操作[内部]，注意：arrayValue!=arrayInfo，arrayInfo是一个符合数组信息描述结构的vector，而arrayValue只是vector即可 */
    /** 判断数组信息arrayValue中的元素是否用来描述一个XXmap中数组结构的信息 */
    bool isArrayInfo(const std::vector<std::string> &arrayValue);

    /** 判断指定编号index是否在数组信息arrayInfo中为有效的编号 */
    bool isArrayContains(const std::vector<std::string> &arrayInfo, unsigned int index);

    /** 判断index是否为数组信息arrayInfo的下一个有效编号，如：info中有5个元素，那么0-4是有效的编号，则5为下一个有效编号 */
    bool isArrayNext(const std::vector<std::string> &arrayInfo, unsigned int index);

    /** 在数组信息arrayInfo中取指定编号的item，并删除 */
    std::string takeArrayItem(const std::vector<std::string> &arrayInfo, unsigned int index);

    /** 在数组信息arrayInfo中取指定编号的item */
    std::string getArrayItem(const std::vector<std::string> &arrayInfo, unsigned int index);

    /** 在数组信息arrayInfo中设置指定编号的item */
    void setArrayItem(const std::vector<std::string> &arrayInfo, unsigned int index, std::string &item);

    /** 在数组信息arrayInfo中追加一个item，并返回item值 */
    std::string addArrayItem(std::vector<std::string> &arrayInfo);

    /** 在数组信息中追加一个指定的item，如有重复的item值则返回false */
    bool addArrayItem(std::vector<std::string> &arrayInfo, std::string &item);

    /** 创建一个新的数组信息，并分配好指定数量count的item */
    std::vector<std::string> createArrayInfo(unsigned int count);

private:
    XXpath _path;
    XXmap &_map;
    bool _isList;
};
#endif