#include "XXmap.h"
#include "../Common/XXstdStringExtend.h"

#define PATH_DELIMITER              "/" // 路径分隔符
#define KEY_ARRAY_PREFIX            "." // 

#define VALUE_ARRAY_ITEMCOUNT_MIN           3   // 数组信息中项最小值：.,COUNT,MAX,编号 
#define VALUE_ARRAY_PREFIX                  "." // 数组信息的标识，如：1/2/3 : .,COUNT,MAX,0,1,2
#define VALUE_ARRAY_DELIMITER               "," // 数组节点/值的分隔符
    #define VALUE_ARRAY_INDEX_PREFIX        0   // 数组信息中的标识符编号
    #define VALUE_ARRAY_INDEX_COUNT         1   // 数组信息中的数量编号
    #define VALUE_ARRAY_INDEX_MAX           2   // 数组信息中的最大项值编号
    #define VALUE_ARRAY_INDEX_ITEMSTART     3 // 数组信息中的项

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
    std::string key = KEY_ARRAY_PREFIX + std::to_string(index);
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
void XXmapRef::operator=(const std::vector<std::string> &value){
    if (0 == value.size())
        return;
    
    std::string valueString;
    for (auto iter = value.cbegin(); iter != value.cend();){
        valueString += *iter;
        if (++iter != value.cend()){        
            valueString += VALUE_ARRAY_DELIMITER;
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
std::vector<std::string> XXmapRef::toVector() const{
    std::string value = this->toString();
    return XXstdStringExtend::splitToVector(value, VALUE_ARRAY_DELIMITER);
}

/// 私有函数 - <值设置(会进行数组检查)>
void XXmapRef::setValue(const std::string &path, const std::vector<std::string> &value){
    std::string valueString;
    for (auto iter = value.cbegin(); iter != value.cend();){
        valueString += *iter;

        auto nextIter = ++iter;
        if (value.cend() != nextIter){
            valueString += VALUE_ARRAY_DELIMITER;  
        }      
    }
    _map._data.insert(path, valueString);
}
void XXmapRef::setValue(const std::string &path, const std::string &value){
    _map._data.insert(path, value);
}
void XXmapRef::setValue(const std::string &path, const std::string &value){
    XXpath realPath;

    // [1] 路径拆分并遍历
    auto nodes = XXstdStringExtend::splitToList(path, PATH_DELIMITER);
    for (auto nodeIter = nodes.cbegin(); nodeIter != nodes.cend(); nodeIter++)
    {
        // 空节点，不作处理，直接跳过
        if (0 == nodeIter->length()){
            realPath << *nodeIter;
            continue;
        }

        if (nodeIter->substr(0,1) == VALUE_ARRAY_PREFIX){
            // 数组节点编号
            unsigned int index  = std::stoi(nodeIter->substr(1));
            
            // 数组信息
            auto infoIter = _map._data.find(realPath._data);
            if (_map._data.end() == infoIter){
                if(0 != index){
                    printf("[XXmap] LINE:%d error\n", __LINE__);
                    return; 
                }
                    
                std::string nodeIter = addArrayItem(realPath._data);
                if("" == nodeIter){
                    printf("[XXmap] LINE:%d error\n", __LINE__);
                    return;                   
                }
            }
            else{
                std::vector<std::string> infoItems = XXstdStringExtend::splitToVector(nodeIter.second, VALUE_LIST_DELIMITER);
                
            }
        }
        else{
            // 一般节点
            realPath << *nodeIter;
        }
    }
}
std::string XXmapRef::addArrayItem(const std::string &path){
    auto infoIter = _map._data.find(path);
    if (_map._data.end() == infoIter){
        std::string infoValue(VALUE_ARRAY_PREFIX);               // 标识头：'.'
        infoValue += VALUE_ARRAY_DELIMITER + std::to_string(1);  // 分隔符：',' + 数组长度：'1'
        infoValue += VALUE_ARRAY_DELIMITER + std::to_string(0);  // 分隔符：',' + 数组元素节点：'0'
        return "0";
    }
    else{
        std::vector<std::string> infoItems = XXstdStringExtend::splitToVector(infoIter->second, VALUE_LIST_DELIMITER);
        if (infoItems.size() < VALUE_ARRAY_ITEMCOUNT_MIN){
            return "";
        }
        else if(VALUE_ARRAY_PREFIX != infoItems[VALUE_ARRAY_INDEX_PREFIX]){
            return "";
        }
        else{
            std::string nextIndexString = std::to_string(std::stoi(infoItems[VALUE_ARRAY_INDEX_MAX]) + 1);
            std::string countString = std::to_string(std::stoi(infoItems[VALUE_ARRAY_INDEX_COUNT]) + 1);
            infoItems[VALUE_ARRAY_INDEX_MAX] = nextIndexString;
            infoItems[VALUE_ARRAY_INDEX_COUNT] = countString;
            infoItems.push_back(nextIndexString);
            setValue(path, infoItems);
            return nextIndexString;
        }
    }
}
bool XXmapRef::setArrayValue(const std::string &prefixPath, unsigned int index, const std::string &value){
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
            return false;
        }
        
        // [2.2] 组成数组信息节点对应的值（value）
        std::string infoValue(VALUE_ARRAY_PREFIX);               // 标识头：'.'
        infoValue += VALUE_ARRAY_DELIMITER + std::to_string(1);  // 
        infoValue += VALUE_ARRAY_DELIMITER + indexString;

        // [2.3] 组成数组元素的键（key）并写入数组元素数据
        XXpath path(prefixPath);
        path << indexString;
        _map._data.insert(path._data, value);

        // [2.4] 写入数组信息数据
        _map._data.insert(prefixPath, infoValue);
    }
    else{
        // [3.1] 取出数组信息数据，并拆分
        auto infoItems = XXstdStringExtend::splitToVector(infoIter->second, VALUE_ARRAY_DELIMITER);
        if (infoItems.size() <= 2 || VALUE_ARRAY_PREFIX != infoItems[VALUE_ARRAY_INDEX_PREFIX]){
            return false; // 数组信息异常
        }

        // [3.2] 判断数量的合法性
        int count   = std::stoi(infoItems[VALUE_ARRAY_INDEX_COUNT]);
        int max     = std::stoi(infoItems[VALUE_ARRAY_INDEX_MAX]);
        if (count <= 0){
            return false; // 数组长度异常
        }

        // [3.3] 判断index和数组数量的关系
        if (index < count){
            std::string pathNode = infoItems[VALUE_ARRAY_INDEX_ITEMSTART+index];
            XXpath path(prefixPath);
            path << pathNode;
            _map._data.insert(path._data, value);
        }
        else if(index == count){
            std::string maxString = std::to_string(max+1);
            infoItems.push_back(maxString);
            infoItems[VALUE_ARRAY_INDEX_COUNT]   = std::to_string(count+1);
            infoItems[VALUE_ARRAY_INDEX_MAX]     = maxString;
            setValue(prefixPath, infoItems);

            XXpath path(prefixPath);
            path << maxString;
            _map._data.insert(path._data, value);
        }
        else{
            return false; // 数据越界
        }
    }
    return true;
}

/// pragma mark - 私有函数
bool XXmapRef::isArrayInfo(const std::vector<std::string> &arrayValue){
    if(arrayValue.size() < VALUE_ARRAY_ITEMCOUNT_MIN){
        return false;
    }
    else if(VALUE_ARRAY_PREFIX != arrayValue[VALUE_ARRAY_INDEX_PREFIX]){
        return false;
    }
    else if(arrayValue.size()-VALUE_ARRAY_ITEMCOUNT_MIN !=  std::stoi(arrayValue[VALUE_ARRAY_INDEX_COUNT])){
        return false;
    }
    else{
        return true;
    }
}
bool XXmapRef::isArrayContains(const std::vector<std::string> &arrayInfo, unsigned int index){
    return std::stoi(arrayInfo[VALUE_ARRAY_INDEX_COUNT]) > index;
}
std::string XXmapRef::getArrayNode(const std::string &path, unsigned int index){
    auto infoIter = _map._data.find(path);
    if(_map._data.end() == infoIter){
        return "";
    }

    auto infoItems = XXstdStringExtend::splitToVector(infoIter->second, VALUE_ARRAY_DELIMITER);
    if (!isArrayInfo(infoItems)){
        return "";
    }
    
    int count = std::stoi(infoItems[VALUE_ARRAY_INDEX_COUNT]);
    return index >= count ? "" : infoItems[VALUE_ARRAY_INDEX_ITEMSTART + index];
}
