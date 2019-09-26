#include "XXjson.h"
#include "XXjvalue.h"
#include "../Common/XXstdStringExtend.h"

#define ERROR_LOG printf("[XXjson] LINE:%d error\n", __LINE__);
#define VALUE_ARRAY_INFOCOUNT_MIN           2   // (来自XXmap)数组信息中项最小值：.,MAX(,编号) 
#define VALUE_ARRAY_PREFIX                  "." // (来自XXmap)数组信息的标识，如：1/2/3 : .,MAX,0,1,2
#define VALUE_ARRAY_DELIMITER               "," // (来自XXmap)数组节点/值的分隔符

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

        int deepness = XXstdStringExtend::count(workingKey, '/') + 1;   
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
    std::map<std::string, std::vector<int>*> arrayPathToItems;

    auto &mapData = ref.xxmap().data();
    for (auto iter = _jmap.cbegin(); _jmap.cend() != iter; iter++){
        XXjvalue jvalue(iter->second);

        switch (jvalue.type()){
        case XXjvalue::Type::PathNode:
            break;
        case XXjvalue::Type::PathValue:
            mapData[iter->first] = jvalue.value();
            break;
        case XXjvalue::Type::ArrayInfo:{
            auto items = arrayPathToItems.find(iter->first);
            if(arrayPathToItems.end() != items){
                ERROR_LOG
            }
            else{
                
            }   
            break;
        }
        case XXjvalue::Type::ArrayItem:
            break;
        case XXjvalue::Type::ArrayValue:
            mapData[iter->first] = jvalue.value();
            break;
        }
    }
}

std::string XXjson::toString(bool isThin){
    // [1] 预分配空间
    std::string jsonString;
    jsonString.reserve(512);

    int lastDeepness    = 0;
    //int arrayDeepness   = -1;
    std::vector<int> arrayDeepness;
    for (auto iter = _jmap.cbegin(); iter != _jmap.cend(); iter++){
        XXjvalue jvalue(iter->second);

        // [2] 节点值deepness检测
        if (lastDeepness > jvalue.deepness()){
            // [3.1] 比上一个路径深度少，需要添加足够的'}'或者']'
            int diff = lastDeepness - jvalue.deepness();
            for(int index = 1; index <= diff; index++){
                if(!isThin) addTabSpacer(jsonString, lastDeepness-index);
                if(!arrayDeepness.empty() && arrayDeepness.back() == lastDeepness-index){
                    jsonString += isThin ? "]," : "],\n";
                    arrayDeepness.pop_back();
                }
                else{
                    jsonString += isThin ? "}," : "},\n";
                }
            }
        }
        else if (lastDeepness < jvalue.deepness() && (XXjvalue::Type::ArrayItem != jvalue.type() && XXjvalue::Type::ArrayValue != jvalue.type())){
            if(!isThin) addTabSpacer(jsonString, lastDeepness);
            jsonString.append(isThin ? "{" : "{\n");
        }
        else{}

        // [3] 值写入
        if(XXjvalue::Type::PathValue == jvalue.type()){
            if(!isThin) addTabSpacer(jsonString, jvalue.deepness());
            addPair(jsonString, XXstdStringExtend::section(iter->first, "/", -1), jvalue.value());
            if(!isThin) jsonString += "\n";
        }
        else if(XXjvalue::Type::PathNode == jvalue.type()){
            if(!isThin) addTabSpacer(jsonString, jvalue.deepness());
            addPair(jsonString, XXstdStringExtend::section(iter->first, "/", -1));
            if(!isThin) jsonString += "\n";
        }
        else if(XXjvalue::Type::ArrayInfo == jvalue.type()){
            if(!isThin) addTabSpacer(jsonString, jvalue.deepness());
            addPair(jsonString, XXstdStringExtend::section(iter->first, "/", -1));
            jsonString += '[';
            if(!isThin) jsonString += "\n";
            arrayDeepness.push_back(jvalue.deepness());
        }
        else if(XXjvalue::Type::ArrayItem == jvalue.type()){

        }
        else if(XXjvalue::Type::ArrayValue == jvalue.type()){
            if(!isThin) addTabSpacer(jsonString, jvalue.deepness());
            jsonString += '\"' + jvalue.value() + "\",";
            if(!isThin) jsonString += "\n";
        }
        else{

        }

        lastDeepness = jvalue.deepness();
    }

    for(int index = 1; index <= lastDeepness; index++){
        if(!isThin) addTabSpacer(jsonString, lastDeepness-index);
        if(!arrayDeepness.empty() && arrayDeepness.back() == lastDeepness-index){
            jsonString += ']';
            arrayDeepness.pop_back();
        }
        else{
            jsonString += '}';
        }

        if(index != lastDeepness){
            jsonString += isThin ? "," : ",\n";
        }
    }
    return jsonString;
}

void XXjson::addPair(std::string &str, const std::string &key, const std::string &value){
    // 添加键值对："key":"value", 或者 "key":
    str += '\"' + key + "\":";
    if ("" != value){
        str += '\"' + value + "\",";
    }
}
void XXjson::addTabSpacer(std::string &str, uint8_t deepness){
    while(deepness > 0){
        str.append("    ");
        --deepness;
    }
}