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
    std::map<std::string,std::string>& data() { return _data; }
    const std::map<std::string,std::string>& constData() {return _data;}

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

public: /** 路径编辑运算符 */
    XXmapRef operator[](const std::string &key);    /** 一般路径编辑，不能空节点，不能以'.'开头 */
    XXmapRef operator()(unsigned int index);        /** 数组路径编辑 */
    
public: /** 赋值运算符 */
    void operator=(XXmapRef &mapRef);               /** 只会进行值复制，不会产生map的引用 */
    void operator=(const std::string &value);       
    void operator=(int value);
    void operator=(double value);
    void operator=(const std::vector<std::string> &value);  /** 会将数组内容转换成item1,item2,item3,...,itemN */

public: /** 值转换 */
    operator std::string(); /** 省缺类型转换 */
    std::string toString(); /** 路径有误时，或者没有对应的值时，返回XXMAP_VALUE_INVALID */
    int toInt();            /** 路径有误时，或者没有对应的值时，返回0 */
    double toDouble();      /** 路径有误时，或者没有对应的值时，返回0 */
    std::vector<std::string> toVector();    /** 路径有误时，或者没有对应的值时，空的列表 */

public: /** 数组信息操作，item为整型字符串 */
    bool isArray();                                                 /** 判断当前路径是否对应一个数组信息结构 */
    unsigned int arrayItemCount();                                  /** 获取数组信息中item数量 */
    bool swapArrayItem(unsigned int index1, unsigned int index2);
    std::string insertArrayItem(unsigned int index, bool toBack);
    bool insertArrayItem(unsigned int index, std::string &item, bool toBack);

    std::string addArrayItem();
    std::string getArrayItem(unsigned int index);
    std::string takeArrayItem(unsigned int index);
    bool removeArrayItem(unsigned int index);

public:
    void cleanup(); /** 清除当前路径下的数据，包括子路径（_path和_path/...下的所有键值 */
    XXmap& data(){return _map;}
    const XXmap& constData() const { return _map;}
    bool isEmpty(){return 0 == _map._data.size();}

private: /** 值设置[内部] */
    void cleanup(const std::string &path);
    void setValue(const std::string &path, const std::vector<std::string> &value);
    void checkAndSetValue(const std::string &path, const std::string &value);
    std::string checkAndGetValue(const std::string &path);
    bool getRealPath(const std::string &path, std::string &realPath, bool creating);

private: /** 数组信息操作[内部]，注意：arrayValue!=arrayInfo，arrayInfo是一个符合数组信息描述结构的vector，而arrayValue只是vector即可；item是一个整型字符串 */
    bool isArrayInfo(const std::vector<std::string> &arrayValue);                           /** 判断数组信息arrayValue中的元素是否用来描述一个XXmap中数组结构的信息 */    
    bool isArrayContains(const std::vector<std::string> &arrayInfo, unsigned int index);    /** 判断指定编号index是否在数组信息arrayInfo中为有效的编号 */
    bool isArrayNext(const std::vector<std::string> &arrayInfo, unsigned int index);        /** 判断index是否为数组信息arrayInfo的下一个有效编号，如：info中有5个元素，那么0-4是有效的编号，则5为下一个有效编号 */

    std::string takeArrayItem(std::vector<std::string> &arrayInfo, unsigned int index);             /** 在数组信息arrayInfo中取指定编号的item，并删除 */    
    std::string getArrayItem(const std::vector<std::string> &arrayInfo, unsigned int index);        /** 在数组信息arrayInfo中取指定编号的item */    
    void setArrayItem(std::vector<std::string> &arrayInfo, unsigned int index, std::string &item);  /** 在数组信息arrayInfo中设置指定编号的item */
    
    void swapArrayItem(std::vector<std::string> &arrayInfo, unsigned int index1, unsigned int index2);              /** 在数组信息arrayInfo中，将两个指定的编号在顺序替换*/
    bool insertArrayItem(std::vector<std::string> &arrayInfo, unsigned int index, std::string &item, bool toBack);  /** 在数组信息arrayInfo中，往指定的编号[前/后]插入指定的item (toBack?前插入:后插入) */
    std::string insertArrayItem(std::vector<std::string> &arrayInfo, unsigned int index, bool toBack);              /** 在数组信息arrayInfo中，往指定的编号[前/后]插入由内部生成item，并返回 (toBack?前插入:后插入) */              

    std::string addArrayItem(std::vector<std::string> &arrayInfo);              /** 在数组信息arrayInfo中追加一个item，并返回item值 */
    bool addArrayItem(std::vector<std::string> &arrayInfo, std::string &item);  /** 在数组信息中追加一个指定的item，如有重复的item值则返回false */
    std::vector<std::string> createArrayInfo(unsigned int count);               /** 创建一个新的数组信息，并分配好指定数量count的item */

private:
    XXpath _path;
    XXmap &_map;
};
#endif