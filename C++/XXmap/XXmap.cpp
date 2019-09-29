#include "XXmap.h"

/** 异常、错误标志 */
#define ERROR_LOG printf("[XXmap] LINE:%d error\n", __LINE__);
/** 如果当前路径下不是数组节点则返回false（使用了isArrayInfo） */
#define RETURN_VALUE_IFNOT_ARRAY(mapIter, arrayInfo, value)\
                                    auto mapIter = _map._data.find(_path._data); \
                                    if(_map._data.end() == mapIter) return value;   \
                                    auto arrayInfo = XXstdStringExtend::splitToVector(mapIter->second, VALUE_ARRAY_DELIMITER); \
                                    if(!isArrayInfo(arrayInfo)) return value;
/** 如果当前路径下不是数组节点则直接返回 （使用了isArrayInfo）*/
#define RETURN_IFNOT_ARRAY(mapIter, arrayInfo)\
                                    auto mapIter = _map._data.find(_path._data); \
                                    if(_map._data.end() == mapIter) return;   \
                                    auto arrayInfo = XXstdStringExtend::splitToVector(mapIter->second, VALUE_ARRAY_DELIMITER); \
                                    if(!isArrayInfo(arrayInfo)) return;

#define PATH_DELIMITER              "/" // 路径分隔符
#define KEY_ARRAY_PREFIX            "." // (n)-->.n保存在路径中，其中n是整型字符串

#define VALUE_ARRAY_INFOCOUNT_MIN           2   // 数组信息中项最小值：.,MAX(,编号) 
#define VALUE_ARRAY_PREFIX                  "." // 数组信息的标识，如：1/2/3 : .,MAX,0,1,2
#define VALUE_ARRAY_DELIMITER               "," // 数组节点/值的分隔符
    #define VALUE_ARRAY_INDEX_PREFIX        0   // 数组信息中的标识符编号
    #define VALUE_ARRAY_INDEX_MAX           1   // 数组信息中的最大项值编号
    #define VALUE_ARRAY_INDEX_ITEMSTART     2   // 数组信息中的项

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

/// 
bool XXmapRef::isArray(){
    auto iter = _map._data.find(_path._data);
    if(_map._data.end() == iter){
        return false;
    }
    auto arrayValue = XXstdStringExtend::splitToVector(iter->second, VALUE_ARRAY_DELIMITER);
    return isArrayInfo(arrayValue);
}
unsigned int XXmapRef::arrayItemCount(){
    auto iter = _map._data.find(_path._data);
    if(_map._data.end() == iter){
        return 0;
    }
    auto arrayValue = XXstdStringExtend::splitToVector(iter->second, VALUE_ARRAY_DELIMITER);
    if(!isArrayInfo(arrayValue)){
        return 0;
    }
    return arrayValue.size()-VALUE_ARRAY_INFOCOUNT_MIN;
}

void XXmapRef::createArray(const std::vector<int> &items){
    if(items.empty())
        return;

    std::vector<std::string> arrayInfo;
    arrayInfo.push_back(VALUE_ARRAY_PREFIX);
    arrayInfo.push_back(std::to_string(items.back()));
    for (int index = 0; index < items.size(); index++){
        arrayInfo.push_back(std::to_string(items[index]));
    }
    setValue(_path._data, arrayInfo);
}
bool XXmapRef::swapArrayItem(unsigned int index1, unsigned int index2){
    RETURN_VALUE_IFNOT_ARRAY(mapIter, arrayInfo, false)
    std::string item1 = getArrayItem(arrayInfo, index1);
    std::string item2 = getArrayItem(arrayInfo, index2);
    setArrayItem(arrayInfo, index1, item2);
    setArrayItem(arrayInfo, index2, item1);
    setValue(_path._data, arrayInfo);
    return true;
}
std::string XXmapRef::insertArrayItem(unsigned int index, bool toBack){
    RETURN_VALUE_IFNOT_ARRAY(mapIter, arrayInfo, XXMAP_VALUE_INVALID)
    std::string item = insertArrayItem(arrayInfo, index, toBack);
    if (XXMAP_VALUE_INVALID != item){
         setValue(_path._data, arrayInfo);
    }
    return item;
}
bool XXmapRef::insertArrayItem(unsigned int index, std::string &item, bool toBack){
    RETURN_VALUE_IFNOT_ARRAY(mapIter, arrayInfo, false)
    if(!insertArrayItem(arrayInfo, index, item, toBack)){
        return false;
    }
    setValue(_path._data, arrayInfo);
    return true;
}

std::string XXmapRef::addArrayItem(){
    RETURN_VALUE_IFNOT_ARRAY(mapIter, arrayInfo, XXMAP_VALUE_INVALID)
    std::string item = addArrayItem(arrayInfo);
    if (XXMAP_VALUE_INVALID != item){
        setValue(_path._data, arrayInfo);
    }
    return item;
}
void XXmapRef::addArrayItem(std::string &item){
    RETURN_IFNOT_ARRAY(mapIter, arrayInfo);
    if(addArrayItem(arrayInfo, item)){
        setValue(_path._data, arrayInfo);
    }
}
std::string XXmapRef::getArrayItem(unsigned int index){
    RETURN_VALUE_IFNOT_ARRAY(mapIter, arrayInfo, XXMAP_VALUE_INVALID)
    return getArrayItem(arrayInfo, index);
}
std::string XXmapRef::takeArrayItem(unsigned int index){
    RETURN_VALUE_IFNOT_ARRAY(mapIter, arrayInfo, XXMAP_VALUE_INVALID)
    std::string item = takeArrayItem(arrayInfo, index);
    if (XXMAP_VALUE_INVALID != item){
        setValue(_path._data, arrayInfo);
    }
    return item;
}
bool XXmapRef::removeArrayItem(unsigned int index){
    RETURN_VALUE_IFNOT_ARRAY(mapIter, arrayInfo, false)
    std::string item = takeArrayItem(arrayInfo, index);
    if (XXMAP_VALUE_INVALID == item){
        return false;
    }

    // 回写arrayInfo数据
    setValue(_path._data, arrayInfo);

    // 删除item节点数据
    XXpath itemPath = _path;
    itemPath << item;
    cleanup(itemPath._data);
    return true;
}

void XXmapRef::cleanup(){
    cleanup(_path._data);
}

/// 私有函数 - <节点清除> <数组型值设置> <值写入(会检查路径节点)> <值读取(会检查路径节点)> <路径节点替换>
void XXmapRef::cleanup(const std::string &path){
    if("" == path){
        _map._data.clear();
        return;
    }

    std::string brunchPath = path + "/";
    auto iter = _map._data.begin();
    while (_map._data.end() != iter){
        if(path == iter->first || XXstdStringExtend::hasPrefix(iter->first, brunchPath)){
            iter = _map._data.erase(iter);
        }
        else{
            ++iter;
        }
        
    }
}
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
        // if (0 == nodeIter->length()){
        //     xxRealPath << *nodeIter;
        //     continue;
        // }

        // [2.2] 具有.index的节点
        if (nodeIter->substr(0,1) == VALUE_ARRAY_PREFIX){
            // [3.1] 数组节点编号
            unsigned int index  = std::stoi(nodeIter->substr(1));
            
            // [3.2] 数组信息
            auto infoIter = _map._data.find(xxRealPath._data);
            if (_map._data.end() == infoIter){
                // 没有数组信息，并index不为0，不能创建item
                if(0 != index || !creating){
                    ERROR_LOG
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
                    ERROR_LOG
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
                    ERROR_LOG
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
    else{
        return true;
    }
}
bool XXmapRef::isArrayContains(const std::vector<std::string> &arrayInfo, unsigned int index){
    return (arrayInfo.size()-VALUE_ARRAY_INFOCOUNT_MIN) > index;
}
bool XXmapRef::isArrayNext(const std::vector<std::string> &arrayInfo, unsigned int index){
    return (arrayInfo.size()-VALUE_ARRAY_INFOCOUNT_MIN) == index;
}

std::string XXmapRef::takeArrayItem(std::vector<std::string> &arrayInfo, unsigned int index){
    auto iter = arrayInfo.begin();
    iter += VALUE_ARRAY_INDEX_ITEMSTART+index;
    std::string item = *iter;
    arrayInfo.erase(iter);
    return item;
}
std::string XXmapRef::getArrayItem(const std::vector<std::string> &arrayInfo, unsigned int index){
    return arrayInfo[VALUE_ARRAY_INDEX_ITEMSTART+index];
}
void XXmapRef::setArrayItem(std::vector<std::string> &arrayInfo, unsigned int index, std::string &item){
    arrayInfo[VALUE_ARRAY_INDEX_ITEMSTART+index] = item;
}

void XXmapRef::swapArrayItem(std::vector<std::string> &arrayInfo, unsigned int index1, unsigned int index2){
    std::string item1 = arrayInfo[VALUE_ARRAY_INDEX_ITEMSTART+index1];
    std::string item2 = arrayInfo[VALUE_ARRAY_INDEX_ITEMSTART+index2];
    arrayInfo[index1] = item2;
    arrayInfo[index2] = item1;
}
bool XXmapRef::insertArrayItem(std::vector<std::string> &arrayInfo, unsigned int index, std::string &item, bool toBack){
    auto iter = arrayInfo.begin();
    iter += toBack ? VALUE_ARRAY_INDEX_ITEMSTART+index+1 : VALUE_ARRAY_INDEX_ITEMSTART+index;
    if (arrayInfo.end() == iter){
        if(!toBack) {
            ERROR_LOG
            return false;
        }
        arrayInfo.push_back(item);
    }
    else{
        arrayInfo.insert(iter, item);
    }

    int currentMax  = std::stoi(arrayInfo[VALUE_ARRAY_INDEX_MAX]);
    int itemIndex   = std::stoi(item);
    arrayInfo[VALUE_ARRAY_INDEX_MAX] = std::to_string(std::max(currentMax,itemIndex));
    return true;
}
std::string XXmapRef::insertArrayItem(std::vector<std::string> &arrayInfo, unsigned int index, bool toBack){
    std::string newItem = std::to_string(std::stoi(arrayInfo[VALUE_ARRAY_INDEX_MAX])+1);
    if(!insertArrayItem(arrayInfo, index, newItem, toBack)){
        return XXMAP_VALUE_INVALID;
    }

    arrayInfo[VALUE_ARRAY_INDEX_MAX] = newItem;
    return newItem;
}

std::string XXmapRef::addArrayItem(std::vector<std::string> &arrayInfo){
    std::string max = std::to_string(std::stoi(arrayInfo[VALUE_ARRAY_INDEX_MAX]) + 1);
    arrayInfo[VALUE_ARRAY_INDEX_MAX] = max;
    arrayInfo.push_back(max);
    return max;
}
bool XXmapRef::addArrayItem(std::vector<std::string> &arrayInfo, const std::string &item){
    auto iter = arrayInfo.begin()+VALUE_ARRAY_INDEX_ITEMSTART;
    while(iter != arrayInfo.end()){
        if(*iter == item){
            return false;
        }
    }

    int itemIndex   = std::stoi(item);
    int max         = std::stoi(arrayInfo[VALUE_ARRAY_INDEX_MAX]);
    if(itemIndex > max){
        arrayInfo[VALUE_ARRAY_INDEX_MAX] = item;
    }
    arrayInfo.push_back(item);
    return true;
}
std::vector<std::string> XXmapRef::createArrayInfo(unsigned int count){
    std::vector<std::string> arrayInfo;
    arrayInfo.resize(VALUE_ARRAY_INFOCOUNT_MIN + 1);
    arrayInfo[VALUE_ARRAY_INDEX_PREFIX] = VALUE_ARRAY_PREFIX;
    arrayInfo[VALUE_ARRAY_INDEX_MAX]    = std::to_string(count>0?count-1:-1);

    for (int index = 0; index < count; index++){
        arrayInfo[VALUE_ARRAY_INDEX_ITEMSTART+index] = (std::to_string(index));
    }
    return arrayInfo;
}