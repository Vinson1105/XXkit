#include "XXmap.h"
#include "../Common/XXstdStringExtend.h"

#define PATH_DELIMITER          '/'     // 路径分隔符
#define KEY_LIST_PREFIX             '.' // 

#define VALUE_LIST_ITEMCOUNT_MIN    3   // 数组信息中项最小值：.,COUNT,MAX 
#define VALUE_LIST_PREFIX           '.' // 数组信息的标识，如：1/2/3 : .,COUNT,MAX,0,1,2
#define VALUE_LIST_DELIMITER        ',' // 数组节点/值的分隔符
    #define VALUE_LIST_INDEX_PREFIX 0   // 数组信息中的标识符编号
    #define VALUE_LIST_INDEX_COUNT  1   // 数组信息中的数量编号
    #define VALUE_LIST_INDEX_MAX    2   // 数组信息中的最大项值编号
    #define VALUE_LIST_INDEX_ITEMSTART  3 // 数组信息中的项

/// 构建析构函数
XXmapRef::XXmapRef(XXmap &map)
    :_map(map){}
XXmapRef::XXmapRef(const XXmapRef &mapRef)
    : _map(mapRef._map)
    , _path(mapRef._path){}

/// 公共函数 - 路径编辑
XXmapRef XXmapRef::operator[](const std::string &key){
    XXmapRef mapRef(*this);
    mapRef._path << key;
    return mapRef;
}
XXmapRef XXmapRef::operator()(unsigned int index){
    XXmapRef mapRef(*this);
    std::string key = "." + std::to_string(index);
    mapRef._path << key;
    return mapRef;
}

/// 公共函数 - 赋值函数
void XXmapRef::operator=(const std::string &value){
    if(XXMAP_VALUE_INVALID == value)
        return;
    _map._data.insert(_path._data, value);
}
void XXmapRef::operator=(int value){
    _map._data.insert(_path._data, std::to_string(value));
}
void XXmapRef::operator=(double value){
    _map._data.insert(_path._data, std::to_string(value));
}
void XXmapRef::operator=(const std::list<std::string> &value){
    if (0 == value.size())
        return;
    
    std::string valueString;
    for (auto iter = value.cbegin(); iter != value.cend();){
        valueString += *iter;
        if (++iter != value.cend()){        
            valueString += VALUE_LIST_DELIMITER;
        }
    }

    _map._data.insert(_path._data, valueString);
}
void XXmapRef::operator=(const std::vector<std::string> &value){
    if (0 == value.size())
        return;
    
    std::string valueString;
    for (auto iter = value.cbegin(); iter != value.cend();){
        valueString += *iter;
        if (++iter != value.cend()){        
            valueString += VALUE_LIST_DELIMITER;
        }
    }

    _map._data.insert(_path._data, valueString);
}

/// 公共函数 - 取值函数
XXmapRef::operator std::string() const{
    return this->toString();
}
std::string XXmapRef::toString() const{
    auto iter = _map._data.find(_path._data);
    if (_map._data.end() == iter){
        return XXMAP_VALUE_INVALID;
    }
    return iter->second;
}
int XXmapRef::toInt() const{
    return std::stoi(this->toString());
}
double XXmapRef::toDouble() const{
    return std::stod(this->toString());
}
std::list<std::string> XXmapRef::toList() const{
    std::string value = this->toString();
    return XXstdStringExtend::splitToList(value, VALUE_LIST_DELIMITER);
}

/// 私有函数 - <值设置(会进行数组检查)>
void XXmapRef::setValue(const std::string &value){
    XXpath realPath;

    // 路径拆分并遍历
    auto nodeList = XXstdStringExtend::splitToList(_path._data, PATH_DELIMITER);
    for (auto nodeIter = nodeList.cbegin(); nodeIter != nodeList.cend(); nodeIter++)
    {
        // 空节点
        if (0 == nodeIter->length()){
            realPath << *nodeIter;
            continue;
        }

        auto charIter = nodeIter->cbegin();
        if (*charIter == VALUE_LIST_PREFIX){
            // 数组节点编号
            unsigned int index  = std::stoi(std::string(++charIter,nodeIter->cend()));
            
            // 数组信息
            int count = -1;

            auto listInfoIter = _map._data.find(realPath._data);
            if(_map._data.end() != listInfoIter){
                
            }

            // 数组管理节点值
            if (_map._data.end() == listInfoIter){
                std::string value(1, VALUE_LIST_PREFIX);
                value += VALUE_LIST_DELIMITER;
                value += 
                _map._data.insert(realPath, );
            }
            else{

            }
        }
        else{
            // 一般节点
            realPath << *nodeIter;
        }
    }
}
void XXmapRef::setArrayValue(const std::string &prefixPath, unsigned int index, const std::string &value){
    /**
     * 数组信息节点组成：".,Count,Max,Index"，其中：
     * ','：为分隔符；
     * '.'：为数组信息数据标识；
     * 'Count'：为数组长度；
     * 'Max'：实际数组元素在map中的路径最大节点值，后续增加节点需要在此值的基础上递增，并且此值只增不减
     * 'Index'：实际数组元素在map中的路径节点
    */
    
    // [1] 取出数组信息节点
    std::string indexString = std::to_string(index);
    auto infoIter = _map._data.find(prefixPath);
    if (_map._data.end() == infoIter){
        // [2.1] 没能找到信息节点的迭代器（即表中没有信息节点值），此时如果index==0，则可以视为数组的初始化
        if (0 != index){
            return;
        }
        
        // [2.2] 组成数组信息节点对应的值（value）
        std::string infoValue(1, VALUE_LIST_PREFIX);            // 标识头：'.'
        infoValue += VALUE_LIST_DELIMITER + std::to_string(1);  // 
        infoValue += VALUE_LIST_DELIMITER + indexString;

        // [2.3] 组成数组元素的键（key）并写入数组元素数据
        XXpath path(prefixPath);
        path << indexString;
        _map._data.insert(path._data, value);

        // [2.4] 写入数组信息数据
        _map._data.insert(prefixPath, infoValue);
    }
    else{
        // [3.1] 取出数组信息数据，并拆分
        auto info       = infoIter->second;
        auto infoNodes  = XXstdStringExtend::splitToVector(info, VALUE_LIST_DELIMITER);
        if (infoNodes.size() <= 2 || std::string(1,VALUE_LIST_PREFIX) != infoNodes[VALUE_LIST_INDEX_PREFIX]){
            return;
        }

        // [3.2] 判断数量的合法性
        int count   = std::stoi(infoNodes[VALUE_LIST_INDEX_COUNT]);
        int max     = std::stoi(infoNodes[VALUE_LIST_INDEX_MAX]);
        if (count <= 0){
            return; // 异常数量
        }

        // [3.3] 判断index和数组数量的关系
        if (index < count){
            std::string pathNode = infoNodes[VALUE_LIST_INDEX_ITEMSTART+index];
            XXpath path(prefixPath);
            path << pathNode;
            _map._data.insert(path._data, value);
        }
        else if(index == count){
            std::string maxString = std::to_string(max+1);
            infoNodes.push_back(maxString);
            infoNodes[VALUE_LIST_INDEX_COUNT]   = std::to_string(count+1);
            infoNodes[VALUE_LIST_INDEX_MAX]     = maxString;
        }
        else{
            return;
        }
    }
}

