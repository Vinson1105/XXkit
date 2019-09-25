#include "XXjson.h"
#include "XXjvalue.h"
#include "../Common/XXstdStringExtend.h"

#define VALUE_ARRAY_INFOCOUNT_MIN           2   // (来自XXmap)数组信息中项最小值：.,MAX(,编号) 
#define VALUE_ARRAY_PREFIX                  "." // (来自XXmap)数组信息的标识，如：1/2/3 : .,MAX,0,1,2
#define VALUE_ARRAY_DELIMITER               "," // (来自XXmap)数组节点/值的分隔符

void XXjson::fromMap(XXmapRef ref){
    _jmap.clear();
    auto &mapData = ref.xxmap().constData();
    if(0 == mapData.size()){
        return;
    }

    for (auto iter = mapData.begin(); iter != mapData.end(); iter++)
    {
        // [1] 判断该键值对是普通的value还是arrayInfoValue
        std::string workingKey = iter->first;

        int deepness = XXstdStringExtend::count(workingKey, '/') + 1;   
        int countOfArrayItem = 0;     
        bool isArrayInfo = false;
        if('.' == iter->second.at(0) && ',' == iter->second.at(1)){
            auto arrayInfo = XXstdStringExtend::splitToVector(iter->second, ',');
            countOfArrayItem = arrayInfo.size() - VALUE_ARRAY_INFOCOUNT_MIN;
            isArrayInfo = true;
        }

        // [2] 保存value节点
        XXjvalue jvalue;
        if(isArrayInfo){
            jvalue = XXjvalue(deepness, XXjvalue::Type::ArrayInfo);
        }
        else{
            jvalue = XXjvalue(deepness, XXjvalue::Type::ValueItem, iter->second);
        }
        _jmap[workingKey] = jvalue.toData();
        if(--deepness <= 0){
            continue;
        }

        // [3] 每次循环删除<workingKey>最后的"/+"路径，并将workingKey以Node方式写入本地jmap中
        for(; deepness > 0; deepness--){
            workingKey.resize(XXstdStringExtend::lastIndexOf(workingKey, '/') - 1);

            auto jmapIter = _jmap.find(workingKey);
            if (jmapIter != _jmap.end()){
                XXjvalue tempJvalue(jmapIter->second);
                if (XXjvalue::Type::ValueItem == tempJvalue.type())
                {
                    jvalue.type(XXjvalue::Type::PathNode);
                    jmapIter->second = jvalue.toData();
                }
            }
            else{
                _jmap[workingKey] = XXjvalue(deepness, XXjvalue::Type::PathNode).toData();
            }
        }
    }
}
// void JSon::toPaMap(PaMapRef paMapRef, const QString &relPath)
// {
//     QMap<QString, QString>& jmap = jmap_.data();
//     QString path = relPath;
//     if (!relPath.isEmpty() && '/' != relPath[relPath.size() - 1])
//         path += '/';

//     JValue jvalue;
//     for (QMap<QString, QString>::iterator iter = jmap.begin(); iter != jmap.end(); iter++)
//     {
//         jvalue.fromData(iter.value());
//         if (jvalue.value().isEmpty())
//             continue;

//         QString key = iter.key();
//         paMapRef.set(path + key, jvalue.value());
//     }
// }

std::string XXjson::toString(bool isThin){
    // [1] 预分配空间
    std::string jsonString;
    jsonString.reserve(512);

    int lastDeepness    = 0;
    int arrayDeepness   = -1;
    for (auto iter = _jmap.cbegin(); iter != _jmap.cend(); iter++){
        XXjvalue jvalue(iter->second);

        // [2] 节点值deepness检测
        if (lastDeepness > jvalue.deepness()){
            // [3.1] 比上一个路径深度少，需要添加足够的'}'或者']'
            int diff = lastDeepness - jvalue.deepness();
            for(int index = 1; index <= diff; index++){
                if(!isThin) addTabSpacer(jsonString, lastDeepness-index);
                if(arrayDeepness == lastDeepness-index){
                    jsonString += isThin ? "]," : "],\n";
                    arrayDeepness = -1;
                }
                else{
                    jsonString += isThin ? "}," : "},\n";
                }
            }

            // [3.2] 如果是数组的下级节点
            if (arrayDeepness >= 0 && 1 == jvalue.deepness() - arrayDeepness){
                lastDeepness = jvalue.deepness();
                continue;
            }
        }
        else if (lastDeepness < jvalue.deepness()){
            int diff = jvalue.deepness() - arrayDeepness;
            if (arrayDeepness >= 0 && 1 == diff ){
                lastDeepness = jvalue.deepness();
                if(XXjvalue::Type::ValueItem != jvalue.type()){
                    continue;
                }
            }
            else{
                if(!isThin) addTabSpacer(jsonString, lastDeepness);
                jsonString.append(isThin ? "{" : "{\n");
            }
        }
        else{}

        // [3] 值写入
        if(XXjvalue::Type::ValueItem == jvalue.type()){
            if(!isThin) addTabSpacer(jsonString, jvalue.deepness());
            if (arrayDeepness >= 0 && 1 == abs(arrayDeepness - jvalue.deepness())){
                jsonString += '\"' + jvalue.value() + "\",";
            }
            else{
                addPair(jsonString, XXstdStringExtend::section(iter->first, "/", -1), jvalue.value());
            }
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
            arrayDeepness = jvalue.deepness();
        }
        else{

        }

        lastDeepness = jvalue.deepness();
    }

    for(int index = 1; index <= lastDeepness; index++){
        if(!isThin) addTabSpacer(jsonString, lastDeepness-index);
        if(arrayDeepness == lastDeepness-index){
            jsonString += ']';
            arrayDeepness = -1;
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