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

    operator std::map<std::string,std::string>(){return _data;}
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

public: /* 路径编辑运算符 */
    XXmapRef operator[](const std::string &key);
    XXmapRef operator()(unsigned int index);
    
public: /* 赋值运算符 */
    void operator=(const std::string &value);
    void operator=(int value);
    void operator=(double value);
    void operator=(const std::vector<std::string> &value);

public: /* 值转换 */
    operator std::string() const;
    std::string toString() const;
    int toInt() const;
    double toDouble() const;
    std::vector<std::string> toVector() const;

private: /* 值设置[内部]*/
    void setValue(const std::string &path, const std::vector<std::string> &value);
    void setValue(const std::string &path, const std::string &value);
    std::string addArrayItem(const std::string &prefixPath);
    bool setArrayValue(const std::string &prefixPath, unsigned int index, const std::string &value);

private:
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
     * @brief   取数据表中以path为键的数据，并以这分数据作为一个数组描述对象，
     *          取出指点index的中间节点
     * @param   path    目标路径
     * @param   index   数组中的编号
     * @return  返回非空字符串：成功取出对应的中间节点，否则该路径对应的数据并不是用来描述数据，
     *          或者编号index不在可取值的范围
    */
    std::string getArrayNode(const std::string &path, unsigned int index);


private:
    XXpath _path;
    XXmap &_map;
    bool _isList;
};
#endif