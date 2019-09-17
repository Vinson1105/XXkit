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
    XXmapRef operator[](const std::string &key);
    XXmapRef operator()(unsigned int index);
    
public: /* 赋值运算符 */
    /** 只会进行值复制，不会产生map的引用 */
    void operator=(XXmapRef &mapRef);
    void operator=(const std::string &value);
    void operator=(int value);
    void operator=(double value);
    void operator=(const std::vector<std::string> &value);

public: /* 值转换 */
    
    operator std::string(); /** 省缺类型转换 */
    std::string toString(); /** 路径有误时，或者没有对应的值时，返回XXMAP_VALUE_INVALID */
    int toInt();            /** 路径有误时，或者没有对应的值时，返回0 */
    double toDouble();      /** 路径有误时，或者没有对应的值时，返回0 */
    std::vector<std::string> toVector();    /** 路径有误时，或者没有对应的值时，空的列表 */

private: /* 值设置[内部]*/
    void setValue(const std::string &path, const std::vector<std::string> &value);
    void checkAndSetValue(const std::string &path, const std::string &value);
    std::string checkAndGetValue(const std::string &path);
    bool getRealPath(const std::string &path, std::string &realPath, bool creating);

private: /* 数组信息操作[内部] */
    /**
     * @brief   判断infoItems中的元素是否用来描述一个XXmap中ArrayValue的信息
     * @param   arrayValue  需要判断的数组型数据
     * @return  返回true：这是一个数组信息描述对象，否则这不是用来描述数组
    */
    bool isArrayInfo(const std::vector<std::string> &arrayValue);

    /**
     * @brief   判断指定编号index是否在arrayInfo中为有效的编号，注意：arrayInfo需要为一个有效的数组信息描述对象
     * @param   index   目标编号
     * @return  返回true：这是一个有效编号
    */
    bool isArrayContains(const std::vector<std::string> &arrayInfo, unsigned int index);

    /**
     * @brief   判断指定编号index是否在arrayInfo中不是一个有效的编号，但是在arrayInfo中是下一个有效编号，
     *          如：array中有5个元素，那么0-4是有效的编号，则5为下一个有效编号
     * @param   index   目标编号
     * @return  返回true：是下一个有效编号
    */
    bool isArrayNext(const std::vector<std::string> &arrayInfo, unsigned int index);

    /**
     * @brief   在数组中取指定的项
     * @param   index   目标编号
     * @return  返回true：是下一个有效编号
    */
    std::string getArrayItem(const std::vector<std::string> &arrayInfo, unsigned int index);

    /**
     * @brief   取数据表中以path为键的数据，并以这分数据作为一个数组描述对象，
     *          取出指点index的中间节点
     * @param   path    目标路径
     * @param   index   数组中的编号
     * @return  返回非空字符串：成功取出对应的中间节点，否则该路径对应的数据并不是用来描述数据，
     *          或者编号index不在可取值的范围
    */
    std::string getArrayItem(const std::string &path, unsigned int index);

    /**
     * @brief   在数组信息中追加一个item
     * @param   arrayInfo   目标数组信息
     * @return  返回产生的item
    */
    std::string addArrayItem(std::vector<std::string> &arrayInfo);

    /**
     * @brief   在数组信息中追加一个item
     * @param   arrayInfo   目标数组信息
     * @return  返回非空字符串：为追加产生的item，否则追加失败
    */
    std::vector<std::string> createArrayInfo(unsigned int count);

private:
    XXpath _path;
    XXmap &_map;
    bool _isList;
};
#endif