#include "XXjson.h"
#include "XXjvalue.h"
#include "../Extend/XXstdStringExtend.h"
#include <map>
#include <string>

#define ERROR_LOG printf("[XXjson] LINE:%d error\n", __LINE__);
#define VALUE_ARRAY_INFOCOUNT_MIN           2   // (来自XXmap)数组信息中项最小值：.,MAX(,编号) 
#define VALUE_ARRAY_PREFIX                  "." // (来自XXmap)数组信息的标识，如：1/2/3 : .,MAX,0,1,2
#define VALUE_ARRAY_DELIMITER               "," // (来自XXmap)数组节点/值的分隔符

#define IS_ON_INDEX(index1,index2)      (index1>=0&&index2>=0)  // 是否为闭区域
#define IS_WAIT_INDEX(index1,index2)    (index1>=0&&index2<0)   // 是否为半闭区域
#define IS_OFF_INDEX(index1,index2)     (index1<0&&index2<0)    // 是否为开区域

// #define IS_ALPHABET(c)  ((c>='A'&&c<='Z') || (c>='a'&&c<='z'))  
// #define IS_NUMBER(n)    (n>='0' && n<=9)
// #define IS_HEX(data)    ('\\'==data[0] && 'x'<=data[1] && IS_ALPHABET(data[2]) && IS_ALPHABET(data[3]))
// #define IS_OCT(data)    ('\\'==data[0] && IS_NUMBER(data[1]) && IS_NUMBER(data[2]) && IS_NUMBER(data[3]))

static void cocktailSort(std::vector<int> &arr);
static void swap(std::vector<int> &arr, int i, int j);

void XXjson::fromMap(XXmapRef ref){
    _jmap.clear();
    auto &mapData = ref.xxmap().constData();
    if(0 == mapData.size()){
        return;
    }

    std::map<std::string,int> arrayItemMap; // 用于保存数组item路径，如有arrayInfo路径为：Class/Array，则Class
    for (auto iter = mapData.begin(); iter != mapData.end(); iter++)
    {
        // [1] 判断该键值对是普通的value还是arrayInfoValue
        std::string workingKey = iter->first;

        int deepness = XXstdStringExtend::count(workingKey, '/');//-1;   
        bool isArrayInfo = false;
        if('.' == iter->second.at(0) && ',' == iter->second.at(1)){
            auto arrayInfo = XXstdStringExtend::splitToVector(iter->second, ',');
            std::string workingPath = workingKey + '/';

            // [1.1] 将arrayItem全部列举，如有arrayInfo路径为：Class/Array，则需要列举Class/Array/0、Class/Array/1、Class/Array/2
            for (auto iter = arrayInfo.begin() + VALUE_ARRAY_INFOCOUNT_MIN; iter != arrayInfo.end(); iter++){
                arrayItemMap[workingPath + *iter] = 0;
            }
            isArrayInfo = true;
        }

        // [2] 如果是<arrayInfo>则直接写入<arrayInfo>信息到本地<jmap>中，如果不是，则需要判断以下：
        //     如果在<arrayItemMap>中找到对应的记录，则本条<value>是<arrayValue>
        if(isArrayInfo){
            _jmap[workingKey] = XXjvalue(deepness, XXjvalue::Type::ArrayInfo).toData();
        }
        else{
            auto arrayItem = arrayItemMap.find(workingKey);
            if(arrayItemMap.end() != arrayItem){
                _jmap[workingKey] = XXjvalue(deepness, XXjvalue::Type::ArrayValue, iter->second).toData();
                arrayItemMap.erase(arrayItem);
            }
            else{
                _jmap[workingKey] = XXjvalue(deepness, XXjvalue::Type::PathValue, iter->second).toData();
            }
        }
        if(--deepness <= 0){
            continue;
        }

        // [3] 每次循环删除<workingKey>最后的"/+"路径，并将workingKey以Node方式写入本地jmap中，
        //     如有路径为：Class/Array/0，将路径依次最后一个"/xx"节点，即为Class/Array和Class，
        //     并逐个判断是否在<arrayItemMap>有对应信息
        for(; deepness > 0; deepness--){
            // [3.1] 去掉最后的</xxx>
            workingKey.resize(XXstdStringExtend::lastIndexOf(workingKey, '/') - 1);
            if(workingKey.empty()){
                break;
            }

            // [3.2] 
            auto jmapItem = _jmap.find(workingKey);
            if(_jmap.end() == jmapItem){
                // [3.2] 检查是否在<arrayItemMap>中是否有记录
                auto arrayItem = arrayItemMap.find(workingKey);
                if(arrayItemMap.end() !=  arrayItem){
                    _jmap[workingKey] = XXjvalue(deepness, XXjvalue::Type::ArrayItem).toData();
                    arrayItemMap.erase(arrayItem);
                }
                else{
                    _jmap[workingKey] = XXjvalue(deepness, XXjvalue::Type::PathNode).toData();
                }
            }
            else{

            }            
        }
    }
}
void XXjson::toMap(XXmapRef ref){
    std::map< std::string, std::vector<int> > arrayPathToItems;

    auto &mapData = ref.xxmap().data();
    for (auto iter = _jmap.cbegin(); _jmap.cend() != iter; iter++){
        XXjvalue jvalue(iter->second);
        std::string &valueRef = jvalue.value();

        switch (jvalue.type()){
        case XXjvalue::Type::PathNode:
            break;
        case XXjvalue::Type::PathValue:
            mapData[iter->first] = valueRef;
            break;
        case XXjvalue::Type::ArrayInfo:{
                auto items = arrayPathToItems.find(iter->first);
                if(arrayPathToItems.end() != items){
                    ERROR_LOG
                    continue;
                }
                else{
                    arrayPathToItems[iter->first] = std::vector<int>();
                }   
                break;
            }
        case XXjvalue::Type::ArrayItem:{
                int index = XXstdStringExtend::lastIndexOf(iter->first, "/");
                std::string info = iter->first.substr(0, index);
                std::string item = iter->first.substr(index+1);
                auto items = arrayPathToItems.find(info);
                if(arrayPathToItems.end() == items){
                    ERROR_LOG
                    continue;
                }
                else{
                    items->second.push_back(std::stoi(item));
                }
                break;
            }
        case XXjvalue::Type::ArrayValue:{
                int index = XXstdStringExtend::lastIndexOf(iter->first, "/");
                std::string info = iter->first.substr(0, index);
                std::string item = iter->first.substr(index+1);
                auto items = arrayPathToItems.find(info);
                if(arrayPathToItems.end() == items){
                    ERROR_LOG
                    continue;
                }
                else{
                    items->second.push_back(std::stoi(item));
                }

                mapData[iter->first] = jvalue.value();
                break;
            }
        }
    }

    for (auto iter = arrayPathToItems.begin(); iter != arrayPathToItems.end(); iter++){
        if(!iter->second.empty()){
            cocktailSort(iter->second);
        }
        std::vector<std::string> info;
        info.push_back(".");
        info.push_back(std::to_string(iter->second.back()));
        for(auto itemIter = iter->second.begin(); itemIter != iter->second.end(); itemIter++){
            info.push_back(std::to_string(*itemIter));
        }
        ref.set(iter->first, info);
    }
}

std::string XXjson::toString(bool usingThin, bool usingTransferred, unsigned int kvSizeMax){
    // [1] 预分配空间
    std::string jsonString;
    jsonString.reserve(512);
    char *keyBuffer = (char*)malloc(kvSizeMax);     // 用于key、value中的特殊字符转换
    char *valueBuffer = (char*)malloc(kvSizeMax);   // 用于key、value中的特殊字符转换

    int lastDeepness    = 0;
    std::vector<int> arrayDeepness;
    for (auto iter = _jmap.cbegin(); iter != _jmap.cend(); iter++){
        XXjvalue jvalue(iter->second);

        // [2] 节点值deepness检测
        if (lastDeepness > jvalue.deepness()){
            // [3.1] 比上一个路径深度少，需要添加足够的'}'或者']'
            int diff = lastDeepness - jvalue.deepness();
            for(int index = 1; index <= diff; index++){
                if(!usingThin) addTabSpacer(jsonString, lastDeepness-index);
                if(!arrayDeepness.empty() && arrayDeepness.back() == lastDeepness-index){
                    jsonString += usingThin ? "]," : "],\n";
                    arrayDeepness.pop_back();
                }
                else{
                    jsonString += usingThin ? "}," : "},\n";
                }
            }
        }
        else if (lastDeepness < jvalue.deepness() && (XXjvalue::Type::ArrayItem != jvalue.type() && XXjvalue::Type::ArrayValue != jvalue.type())){
            if(!usingThin) addTabSpacer(jsonString, lastDeepness);
            jsonString.append(usingThin ? "{" : "{\n");
        }
        else{}

        // [3] 值写入
        if(usingTransferred){
            toTransferred(keyBuffer, kvSizeMax, XXstdStringExtend::section(iter->first, "/", -1));
            toTransferred(valueBuffer, kvSizeMax, jvalue.value());
        }
        else{
            bzero(keyBuffer, kvSizeMax);
            strcpy(keyBuffer, XXstdStringExtend::section(iter->first, "/", -1).data()); 

            bzero(valueBuffer, kvSizeMax);
            strcpy(valueBuffer, jvalue.value().data());   
        }
        

        if(XXjvalue::Type::PathValue == jvalue.type()){
            if(!usingThin) addTabSpacer(jsonString, jvalue.deepness());
            addPair(jsonString, std::string(keyBuffer), valueBuffer);
            if(!usingThin) jsonString += "\n";
        }
        else if(XXjvalue::Type::PathNode == jvalue.type()){
            if(!usingThin) addTabSpacer(jsonString, jvalue.deepness());
            addPair(jsonString, std::string(keyBuffer));
            if(!usingThin) jsonString += "\n";
        }
        else if(XXjvalue::Type::ArrayInfo == jvalue.type()){
            if(!usingThin) addTabSpacer(jsonString, jvalue.deepness());
            addPair(jsonString, std::string(keyBuffer));
            jsonString += '[';
            if(!usingThin) jsonString += "\n";
            arrayDeepness.push_back(jvalue.deepness());
        }
        else if(XXjvalue::Type::ArrayItem == jvalue.type()){

        }
        else if(XXjvalue::Type::ArrayValue == jvalue.type()){
            if(!usingThin) addTabSpacer(jsonString, jvalue.deepness());
            //addPair(jsonString, "", jvalue.value()); //jsonString += '\"' + jvalue.value() + "\",";
            addPair(jsonString, "", jvalue.value());
            if(!usingThin) jsonString += "\n";
        }
        else{

        }

        lastDeepness = jvalue.deepness();
    }

    for(int index = 1; index <= lastDeepness; index++){
        if(!usingThin) addTabSpacer(jsonString, lastDeepness-index);
        if(!arrayDeepness.empty() && arrayDeepness.back() == lastDeepness-index){
            jsonString += ']';
            arrayDeepness.pop_back();
        }
        else{
            jsonString += '}';
        }

        if(index != lastDeepness){
            jsonString += usingThin ? "," : ",\n";
        }
    }
    free(keyBuffer);
    free(valueBuffer);
    return jsonString;
}
bool XXjson::fromString(const std::string &jsonString){
    _jmap.clear();
    XXpath workingPath;
    workingPath.data().reserve(128);
    int length = jsonString.length();

    int currentDeepness     = 0;    // 当前深度
    int lastQuotation       = -1;   // 上一个非转义的引号偏移
    int lastDeepness        = -1;   // 上一个非转义的引号对应的深度

    int keyBegin    = -1;   // 标记一个key的开始偏移
    int keyEnd      = -1;   // 标记一个key的结束偏移
    int valBegin    = -1;   // 标记一个val的开始偏移
    int valEnd      = -1;   // 标记一个val的结束偏移
    bool isIntVal   = false;

    bool isTransferredBegin = false;            // 是否开始转义
    bool isNeedResetIndex   = false;
    std::map<std::string,int> arrayPathToCount; // 数组路径对应的数组元素个数

    const char *data        = jsonString.data();
    const char *offsetData  = jsonString.data();
    unsigned int jsonLength = jsonString.length();

    for (int offset = 0; offset < jsonLength; ++offset,++offsetData){
        // [0] 重置标记
        if(isNeedResetIndex){
            lastQuotation = keyBegin = keyEnd = valBegin = valEnd = -1;   
            isIntVal = isTransferredBegin = isNeedResetIndex = false;
        }

        // [1] 仅对"\""进行转义的兼容
        if('\\' == *offsetData){
            ++offset;   ++offsetData;   continue;
        }
        else if('{' == *offsetData){
            // [2.1] '{'为key、value中的字符，直接进行下一次循环
            if(IS_WAIT_INDEX(keyBegin,keyEnd) || IS_WAIT_INDEX(valBegin,valEnd)){ continue; }
            
            // [2.2] '{'为第一个'{'，即为json结构的边界，直接增加深度后进行下一次循环
            if(currentDeepness==0){ ++currentDeepness; continue; }

            if(IS_ON_INDEX(keyBegin,keyEnd)){
                // [2.3] 已经有key，即可认为为PathNode类型节点
                workingPath<< fromTransferred(data+keyBegin, keyEnd-keyBegin);             
                _jmap[workingPath.data()] = XXjvalue(currentDeepness, XXjvalue::Type::PathNode).toData();
            }
            else{
                // [2.4] 没有key，ArrayItem类型节点，需要通过arrayPathToCount表中获取当前项数量
                auto iter = arrayPathToCount.find(workingPath);
                if(arrayPathToCount.end() == iter){
                    ERROR_LOG   _jmap.clear();  return false;
                }

                std::string item = std::to_string(iter->second);
                iter->second++;
                workingPath<<item;
                _jmap[workingPath.data()] = XXjvalue(currentDeepness, XXjvalue::Type::ArrayItem).toData();
            }
            ++currentDeepness;
            isNeedResetIndex = true;
        }
        else if('}' == *offsetData){
            // [3.1] '}'为key、value中的字符，直接进行下一次循环 
            if(IS_WAIT_INDEX(keyBegin,keyEnd) || IS_WAIT_INDEX(valBegin,valEnd)){ 
                continue; 
            }

            // [3.2] '}'为最后个'}'，即为json结构的边界，直接减少深度后进行下一次循环
            if(currentDeepness==1){ --currentDeepness; continue; }

            // [3.3] 有可能当前深度最后一对键值结尾没有','，则需要在'}'判断有没有存在可能的key、value
            if(IS_ON_INDEX(keyBegin, keyEnd)) {
                if(!IS_ON_INDEX(valBegin, valEnd)){
                    // 当有可用的key时，value不可用是不符合语法
                    ERROR_LOG   _jmap.clear();  return false;
                }

                std::string key     = fromTransferred(data+keyBegin, keyEnd-keyBegin);
                std::string value   = fromTransferred(data+valBegin, valEnd-valBegin);
                _jmap[(workingPath<key).data()] = XXjvalue(currentDeepness, XXjvalue::Type::PathValue, value).toData();
            }
            workingPath.removeLast();
            --currentDeepness;
            isNeedResetIndex = true;
        }
        else if('[' == *offsetData){
            // [4.1] 字符'['标志着数组的开始，需要构建一个arrayInfo的对象，前提需要有一个完整key，当没有key时说明出现了语法错误
            // if(!IS_ON_INDEX(keyBeginIndex, keyEndIndex)){
            //     ERROR_LOG   _jmap.clear();  return false;
            // }

            // [4.2] 暂时保存到arrayPathToCount中，待收集好对应的数组item的数量，并出现对应']'时再回写arrayInfo数据
            workingPath << fromTransferred(data+keyBegin, keyEnd-keyBegin);
            arrayPathToCount[workingPath] = 0;

            ++currentDeepness;
            isNeedResetIndex = true;
        }
        else if(']' == *offsetData){
            // [5.1] 字符']'标志着数组的结束，有可能当前深度最后一对键值结尾没有','，则需要在']'判断有没有存在可能的key、value
            std::string itemValue = "";
            if(IS_ON_INDEX(valBegin, valEnd)){
                itemValue = fromTransferred(data+valBegin, valEnd-valBegin);
            }

            // [5.2] 检查一下路径有没有对应的信息数量信息
            auto iter = arrayPathToCount.find(workingPath.data());
            if(arrayPathToCount.end() == iter){ 
                ERROR_LOG   _jmap.clear();  return false; 
            }

            // [5.3] 写入值
            if(!itemValue.empty()){
                std::string item = std::to_string(iter->second);
                iter->second++;
                _jmap[(workingPath<item).data()] = XXjvalue(currentDeepness, XXjvalue::Type::ArrayValue, itemValue).toData();
            }
            
            workingPath.removeLast();
            --currentDeepness;
            isNeedResetIndex = true;
        }
        else if(',' == *offsetData){
            // [6.1] ','为key、value中的字符，直接进行下一次循环
            if(IS_OFF_INDEX(keyBegin,keyEnd) || IS_WAIT_INDEX(keyBegin,keyEnd) || IS_WAIT_INDEX(valBegin,valEnd)){ continue; }

            // [6.2] 获取对应的key、value
            std::string key     = IS_ON_INDEX(keyBegin,keyEnd) ? fromTransferred(data+keyBegin, keyEnd-keyBegin) : "";
            std::string value   = IS_ON_INDEX(valBegin,valEnd) ? fromTransferred(data+valBegin, valEnd-valBegin) : "";

            // [6.3] 如果只有key，则可以认为是数组中的数据，需要判断该路径有没有对应的数组元素数量数据
            if(key.empty()){
                ERROR_LOG   _jmap.clear();  return false;
            }
            if(value.empty()){
                auto iter = arrayPathToCount.find(workingPath.data());
                if(arrayPathToCount.end() == iter){
                    ERROR_LOG   _jmap.clear();  return false;
                }  
                std::string item = std::to_string(iter->second);
                iter->second++;
                _jmap[(workingPath<item).data()] = XXjvalue(currentDeepness, XXjvalue::Type::ArrayValue, key).toData();          
            }
            else{
                _jmap[(workingPath<key).data()] = XXjvalue(currentDeepness, XXjvalue::Type::PathValue, value).toData();
            }
            isNeedResetIndex = true;
        }
        else if('\"' == *offsetData){
            if(keyBegin < 0)        keyBegin    = offset+1;  
            else if(keyEnd < 0)     keyEnd      = offset;        
            else if(valBegin < 0 )  valBegin    = offset+1;  
            else                    valEnd      = offset;   
        }
        else if(*offsetData >= '0' && *offsetData <= '9'){
            /**
             * 可能是key中的字符，亦可能是value中字符，但是是value的字符前提是：已经有一个完整的key；
             * 所以当key不完整时，只需要不断continue即可；
             * 当key完整时，还需要判断是valueStr还是valueInt，
             * 所以如果此时没有valueStr的起始'\"'（valueStrStartIndex<0），即为valueInt
            */
            if(!IS_ON_INDEX(keyBegin,keyEnd))    
                continue;
            if(valBegin < 0 && valEnd < 0) {
                valBegin = offset;  valEnd = offset+1;  isIntVal = true;
            }  
            else if(valBegin >= 0 && isIntVal){
                valEnd = offset+1;
            }    
            else{}
        }
        else if(':' == *offsetData){
            
        }
        else{

        }
    }

    for (auto iter = arrayPathToCount.begin(); iter != arrayPathToCount.end(); iter++){
        _jmap[iter->first] = XXjvalue(XXstdStringExtend::count(iter->first, '/'), XXjvalue::Type::ArrayInfo).toData();   
    }
    return true;
}

void XXjson::toTransferred(char *buffer, int bufferSize, const std::string &str){
    bzero(buffer, bufferSize);
    if(str.empty()) 
        return;

    char *bufferPtr     = buffer;
    const char *strPtr  = str.data();
    int length          = str.length();

    for(int index = 0; index < length; ++index,++bufferPtr){
        if('\"' == strPtr[index] || '\\' == strPtr[index]){
            *bufferPtr = '\\';
            ++bufferPtr;
        }
        // else if('\\' == strPtr[index] && !(index+3 < length && (IS_HEX(strPtr) || IS_OCT(strPtr)))){
        //     *bufferPtr = '\\';
        //     ++bufferPtr;
        // }
        // else{

        // }
        *bufferPtr = strPtr[index];
    }
}
std::string XXjson::fromTransferred(const char *data, int length){
    std::string str;
    str.reserve(length);
    bool isTransferredBegin = false;
    for(int index = 0; index < length; index++,data++){

        //if('\\' == *data && !(index+3 < length && (IS_HEX(data) || IS_OCT(data)))){
        if('\\' == *data){
            if(isTransferredBegin){
                isTransferredBegin = false;
            }
            isTransferredBegin = true;
            continue;
        }
        str += *data;
    }
    return str;
}
void XXjson::addPair(std::string &str, const std::string &key, const std::string &value){
    if(key.empty() && value.empty())    return;

    // 添加键值对："key":"value", 或者 "key":
    if(!key.empty()){
        str += '\"' + key + "\":";
    }
    
    if (!value.empty()){
        str += '\"' + value + "\",";
    }
}
void XXjson::addTabSpacer(std::string &str, uint8_t deepness){
    while(deepness > 0){
        str.append("    ");
        --deepness;
    }
}

// 鸡尾酒排序
static void cocktailSort(std::vector<int> &arr){
    if (arr.empty())
        return;

    int left = 0;
    int right = arr.size() - 1;
    while (left < right)
    {
        for (int i = left; i < right; i++)
        {
            if (arr[i] > arr[i + 1])
                swap(arr, i, i + 1);
        }
        right--;
        for (int j = right; j > left; j--)
        {
            if (arr[j-1] > arr[j])
                swap(arr, j-1, j);
        }
        left++;
    }
}
static void swap(std::vector<int> &arr, int i, int j){
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}