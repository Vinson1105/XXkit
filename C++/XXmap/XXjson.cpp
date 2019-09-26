#include "XXjson.h"
#include "XXjvalue.h"
#include "../Common/XXstdStringExtend.h"

#define ERROR_LOG printf("[XXjson] LINE:%d error\n", __LINE__);
#define VALUE_ARRAY_INFOCOUNT_MIN           2   // (����XXmap)������Ϣ������Сֵ��.,MAX(,���) 
#define VALUE_ARRAY_PREFIX                  "." // (����XXmap)������Ϣ�ı�ʶ���磺1/2/3 : .,MAX,0,1,2
#define VALUE_ARRAY_DELIMITER               "," // (����XXmap)����ڵ�/ֵ�ķָ���

void XXjson::fromMap(XXmapRef ref){
    _jmap.clear();
    auto &mapData = ref.xxmap().constData();
    if(0 == mapData.size()){
        return;
    }

    std::map<std::string,int> arrayItemMap; // ���ڱ�������item·��������arrayInfo·��Ϊ��Class/Array����Class
    for (auto iter = mapData.begin(); iter != mapData.end(); iter++)
    {
        // [1] �жϸü�ֵ������ͨ��value����arrayInfoValue
        std::string workingKey = iter->first;

        int deepness = XXstdStringExtend::count(workingKey, '/') + 1;   
        bool isArrayInfo = false;
        if('.' == iter->second.at(0) && ',' == iter->second.at(1)){
            auto arrayInfo = XXstdStringExtend::splitToVector(iter->second, ',');
            std::string workingPath = workingKey + '/';

            // [1.1] ��arrayItemȫ���о٣�����arrayInfo·��Ϊ��Class/Array������Ҫ�о�Class/Array/0��Class/Array/1��Class/Array/2
            for (auto iter = arrayInfo.begin() + VALUE_ARRAY_INFOCOUNT_MIN; iter != arrayInfo.end(); iter++){
                arrayItemMap[workingPath + *iter] = 0;
            }
            isArrayInfo = true;
        }

        // [2] �����<arrayInfo>��ֱ��д��<arrayInfo>��Ϣ������<jmap>�У�������ǣ�����Ҫ�ж����£�
        //     �����<arrayItemMap>���ҵ���Ӧ�ļ�¼������<value>��<arrayValue>
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

        // [3] ÿ��ѭ��ɾ��<workingKey>����"/+"·��������workingKey��Node��ʽд�뱾��jmap�У�
        //     ����·��Ϊ��Class/Array/0����·���������һ��"/xx"�ڵ㣬��ΪClass/Array��Class��
        //     ������ж��Ƿ���<arrayItemMap>�ж�Ӧ��Ϣ
        for(; deepness > 0; deepness--){
            // [3.1] ȥ������</xxx>
            workingKey.resize(XXstdStringExtend::lastIndexOf(workingKey, '/') - 1);

            // [3.2] 
            auto jmapItem = _jmap.find(workingKey);
            if(_jmap.end() == jmapItem){
                // [3.2] ����Ƿ���<arrayItemMap>���Ƿ��м�¼
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
    // [1] Ԥ����ռ�
    std::string jsonString;
    jsonString.reserve(512);

    int lastDeepness    = 0;
    //int arrayDeepness   = -1;
    std::vector<int> arrayDeepness;
    for (auto iter = _jmap.cbegin(); iter != _jmap.cend(); iter++){
        XXjvalue jvalue(iter->second);

        // [2] �ڵ�ֵdeepness���
        if (lastDeepness > jvalue.deepness()){
            // [3.1] ����һ��·������٣���Ҫ����㹻��'}'����']'
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

        // [3] ֵд��
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
    // ��Ӽ�ֵ�ԣ�"key":"value", ���� "key":
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