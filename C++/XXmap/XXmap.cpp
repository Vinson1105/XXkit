#include "XXmap.h"
#include "../Common/XXstdStringExtend.h"

#define PATH_DELIMITER              "/" // 路径分隔符
#define KEY_ARRAY_PREFIX            "." // 

#define VALUE_ARRAY_INFOCOUNT_MIN           3   // 数组信息中项最小值：.,COUNT,MAX(,编号) 
#define VALUE_ARRAY_PREFIX                  "." // 数组信息的标识，如：1/2/3 : .,COUNT,MAX,0,1,2
#define VALUE_ARRAY_DELIMITER               "," // 数组节点/值的分隔符
    #define VALUE_ARRAY_INDEX_PREFIX        0   // 数组信息中的标识符编号
    #define VALUE_ARRAY_INDEX_COUNT         1   // 数组信息中的数量编号
    #define VALUE_ARRAY_INDEX_MAX           2   // 数组信息中的最大项值编号
    #define VALUE_ARRAY_INDEX_ITEMSTART     3   // 数组信息中的项

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
void XXmapRef::operator=(XXmapRef &mapRef){
    std::string value = mapRef.checkAndGetValue(mapRef._path._data);
    if(XXMAP_VALUE_INVALID == value){
        return;
    }
    checkAndSetValue(_path._data, value);
}
void XXmapRef::operator=(const std::string &value){
    if(XXMAP_VALUE_INVALID == value)
        return;
    checkAndSetValue(_path._data, value);
}
void XXmapRef::operator=(int value){
    checkAndSetValue(_path._data, std::to_string(value));
}
void XXmapRef::operator=(double value){
    checkAndSetValue(_path._data, std::to_string(value));
}
void XXmapRef::operator=(const std::vector<std::string> &value){
    if (0 == value.size())
        return;
    
    std::string valueString;
    for (auto iter = value.begin(); iter != value.end();){
        valueString += *iter;
        if (++iter != value.end()){        
            valueString += VALUE_ARRAY_DELIMITER;
        }
    }

    checkAndSetValue(_path._data, valueString);
}

/// 公共函数 - 取值函数
XXmapRef::operator std::string(){
    return this->toString();
}
std::string XXmapRef::toString(){
    return checkAndGetValue(_path._data);
}
int XXmapRef::toInt(){
    std::string value = checkAndGetValue(_path._data);
    return XXMAP_VALUE_INVALID == value ? 0 : std::stoi(value);
}
double XXmapRef::toDouble(){
    std::string value = checkAndGetValue(_path._data);
    return XXMAP_VALUE_INVALID == value ? 0 : std::stod(value);
}
std::vector<std::string> XXmapRef::toVector(){
    std::string value = checkAndGetValue(_path._data);
    return XXMAP_VALUE_INVALID == value ? std::vector<std::string>() : XXstdStringExtend::splitToVector(value, VALUE_ARRAY_DELIMITER);
}

/// 私有函数 - <数组型值设置> <值写入(会检查路径节点)> <值读取(会检查路径节点)> <路径节点替换>
void XXmapRef::setValue(const std::string &path, const std::vector<std::string> &value){
    std::string valueString;
    for (auto iter = value.cbegin(); iter != value.cend();){
        valueString += *iter;

        auto nextIter = ++iter;
        if (value.cend() != nextIter){
            valueString += VALUE_ARRAY_DELIMITER;  
        }      
    }
    _map._data[path] = valueString;
}
void XXmapRef::checkAndSetValue(const std::string &path, const std::string &value){
    std::string realPath;
    if(getRealPath(path, realPath, true)){
        _map._data[realPath] = value;
    }
}
std::string XXmapRef::checkAndGetValue(const std::string &path){
    std::string realPath;
    if(!getRealPath(path, realPath, false)){
        return XXMAP_VALUE_INVALID;
    }
    auto iter = _map._data.find(realPath);
    return _map._data.end() == iter ? XXMAP_VALUE_INVALID : iter->second;
}
bool XXmapRef::getRealPath(const std::string &path, std::string &realPath, bool creating){
    XXpath xxRealPath;

    // [1] 路径拆分并遍历
    auto nodes = XXstdStringExtend::splitToList(path, PATH_DELIMITER);
    for (auto nodeIter = nodes.begin(); nodeIter != nodes.end(); nodeIter++)
    {
        // [2.1] 空节点，不作处理，直接跳过
        if (0 == nodeIter->length()){
            xxRealPath << *nodeIter;
            continue;
        }

        // [2.2] 具有.index的节点
        if (nodeIter->substr(0,1) == VALUE_ARRAY_PREFIX){
            // [3.1] 数组节点编号
            unsigned int index  = std::stoi(nodeIter->substr(1));
            
            // [3.2] 数组信息
            auto infoIter = _map._data.find(xxRealPath._data);
            if (_map._data.end() == infoIter){
                // 没有数组信息，并index不为0，不能创建item
                if(0 != index || !creating){
                    printf("[XXmap] LINE:%d error\n", __LINE__);
                    return false; 
                }

                // 没有数组信息，index为0，创建一个新的数值信息，并附带一个item    
                auto arrayInfo = createArrayInfo(1);
                setValue(xxRealPath._data, arrayInfo);
                xxRealPath << "0";
            }
            else{
                std::vector<std::string> info = XXstdStringExtend::splitToVector(infoIter->second, VALUE_ARRAY_DELIMITER);
                if (!isArrayInfo(info)){
                    printf("[XXmap] LINE:%d error\n"
                            "   Value:%s\n", __LINE__, infoIter->second.data());
                    return false;
                }
                
                if (isArrayContains(info, index)){
                    std::string item = getArrayItem(info, index);
                    xxRealPath << item;
                }
                else if(isArrayNext(info, index) && creating){
                    std::string item = addArrayItem(info);
                    setValue(xxRealPath._data, info);
                    xxRealPath << item;
                }
                else{
                    printf("[XXmap] LINE:%d error\n", __LINE__);
                    return false;
                }
            }
        }
        // [2.3] 其余节点
        else{
            xxRealPath << *nodeIter;
        }
    }

    realPath = xxRealPath._data;
    return true;
}

/// pragma mark - 私有函数
bool XXmapRef::isArrayInfo(const std::vector<std::string> &arrayValue){
    if(arrayValue.size() < VALUE_ARRAY_INFOCOUNT_MIN){
        return false;
    }
    else if(VALUE_ARRAY_PREFIX != arrayValue[VALUE_ARRAY_INDEX_PREFIX]){
        return false;
    }
    else if(arrayValue.size()-VALUE_ARRAY_INFOCOUNT_MIN !=  std::stoi(arrayValue[VALUE_ARRAY_INDEX_COUNT])){
        return false;
    }
    else{
        return true;
    }
}
bool XXmapRef::isArrayContains(const std::vector<std::string> &arrayInfo, unsigned int index){
    return std::stoi(arrayInfo[VALUE_ARRAY_INDEX_COUNT]) > index;
}
bool XXmapRef::isArrayNext(const std::vector<std::string> &arrayInfo, unsigned int index){
    return std::stoi(arrayInfo[VALUE_ARRAY_INDEX_COUNT]) == index;
}
std::string XXmapRef::getArrayItem(const std::vector<std::string> &arrayInfo, unsigned int index){
    return arrayInfo[VALUE_ARRAY_INDEX_ITEMSTART+index];
}
std::string XXmapRef::getArrayItem(const std::string &path, unsigned int index){
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
std::string XXmapRef::addArrayItem(std::vector<std::string> &arrayInfo){
    std::string max = std::to_string(std::stoi(arrayInfo[VALUE_ARRAY_INDEX_MAX]) + 1);
    std::string count = std::to_string(std::stoi(arrayInfo[VALUE_ARRAY_INDEX_COUNT]) + 1);
    
    arrayInfo[VALUE_ARRAY_INDEX_MAX] = max;
    arrayInfo[VALUE_ARRAY_INDEX_COUNT] = count;
    arrayInfo.push_back(max);
    return max;
}
std::vector<std::string> XXmapRef::createArrayInfo(unsigned int count){
    std::vector<std::string> arrayInfo;
    arrayInfo.resize(VALUE_ARRAY_INFOCOUNT_MIN + 1);
    arrayInfo[VALUE_ARRAY_INDEX_PREFIX] = VALUE_ARRAY_PREFIX;
    arrayInfo[VALUE_ARRAY_INDEX_COUNT]  = std::to_string(count);
    arrayInfo[VALUE_ARRAY_INDEX_MAX]    = std::to_string(count>0?count-1:-1);

    for (int index = 0; index < count; index++){
        arrayInfo[VALUE_ARRAY_INDEX_ITEMSTART+index] = (std::to_string(index));
    }
    return arrayInfo;
}