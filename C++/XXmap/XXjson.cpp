#include "XXjson.h"
#include "XXjvalue.h"
#include "../Common/XXstdStringExtend.h"
#include <map>
#include <string>

#define ERROR_LOG printf("[XXjson] LINE:%d error\n", __LINE__);
#define VALUE_ARRAY_INFOCOUNT_MIN           2   // (����XXmap)������Ϣ������Сֵ��.,MAX(,���) 
#define VALUE_ARRAY_PREFIX                  "." // (����XXmap)������Ϣ�ı�ʶ���磺1/2/3 : .,MAX,0,1,2
#define VALUE_ARRAY_DELIMITER               "," // (����XXmap)����ڵ�/ֵ�ķָ���

#define IS_ON_INDEX(index1,index2)      (index1>=0&&index2>=0)  // �Ƿ�Ϊ������
#define IS_WAIT_INDEX(index1,index2)    (index1>=0&&index2<0)   // �Ƿ�Ϊ�������
#define IS_OFF_INDEX(index1,index2)     (index1<0&&index2<0)    // �Ƿ�Ϊ������

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

    std::map<std::string,int> arrayItemMap; // ���ڱ�������item·��������arrayInfo·��Ϊ��Class/Array����Class
    for (auto iter = mapData.begin(); iter != mapData.end(); iter++)
    {
        // [1] �жϸü�ֵ������ͨ��value����arrayInfoValue
        std::string workingKey = iter->first;

        int deepness = XXstdStringExtend::count(workingKey, '/');//-1;   
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
            if(workingKey.empty()){
                break;
            }

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
    // [1] Ԥ����ռ�
    std::string jsonString;
    jsonString.reserve(512);
    char *keyBuffer = (char*)malloc(kvSizeMax);     // ����key��value�е������ַ�ת��
    char *valueBuffer = (char*)malloc(kvSizeMax);   // ����key��value�е������ַ�ת��

    int lastDeepness    = 0;
    std::vector<int> arrayDeepness;
    for (auto iter = _jmap.cbegin(); iter != _jmap.cend(); iter++){
        XXjvalue jvalue(iter->second);

        // [2] �ڵ�ֵdeepness���
        if (lastDeepness > jvalue.deepness()){
            // [3.1] ����һ��·������٣���Ҫ����㹻��'}'����']'
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

        // [3] ֵд��
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

    int currentDeepness     = 0;    // ��ǰ���
    int lastQuotation       = -1;   // ��һ����ת�������ƫ��
    int lastDeepness        = -1;   // ��һ����ת������Ŷ�Ӧ�����

    int keyBegin    = -1;   // ���һ��key�Ŀ�ʼƫ��
    int keyEnd      = -1;   // ���һ��key�Ľ���ƫ��
    int valBegin    = -1;   // ���һ��val�Ŀ�ʼƫ��
    int valEnd      = -1;   // ���һ��val�Ľ���ƫ��
    bool isIntVal   = false;

    bool isTransferredBegin = false;            // �Ƿ�ʼת��
    bool isNeedResetIndex   = false;
    std::map<std::string,int> arrayPathToCount; // ����·����Ӧ������Ԫ�ظ���

    const char *data        = jsonString.data();
    const char *offsetData  = jsonString.data();
    unsigned int jsonLength = jsonString.length();

    for (int offset = 0; offset < jsonLength; ++offset,++offsetData){
        // [0] ���ñ��
        if(isNeedResetIndex){
            lastQuotation = keyBegin = keyEnd = valBegin = valEnd = -1;   
            isIntVal = isTransferredBegin = isNeedResetIndex = false;
        }

        // [1] ����"\""����ת��ļ���
        if('\\' == *offsetData){
            ++offset;   ++offsetData;   continue;
        }
        else if('{' == *offsetData){
            // [2.1] '{'Ϊkey��value�е��ַ���ֱ�ӽ�����һ��ѭ��
            if(IS_WAIT_INDEX(keyBegin,keyEnd) || IS_WAIT_INDEX(valBegin,valEnd)){ continue; }
            
            // [2.2] '{'Ϊ��һ��'{'����Ϊjson�ṹ�ı߽磬ֱ��������Ⱥ������һ��ѭ��
            if(currentDeepness==0){ ++currentDeepness; continue; }

            if(IS_ON_INDEX(keyBegin,keyEnd)){
                // [2.3] �Ѿ���key��������ΪΪPathNode���ͽڵ�
                workingPath<< fromTransferred(data+keyBegin, keyEnd-keyBegin);             
                _jmap[workingPath.data()] = XXjvalue(currentDeepness, XXjvalue::Type::PathNode).toData();
            }
            else{
                // [2.4] û��key��ArrayItem���ͽڵ㣬��Ҫͨ��arrayPathToCount���л�ȡ��ǰ������
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
            // [3.1] '}'Ϊkey��value�е��ַ���ֱ�ӽ�����һ��ѭ��
            if(IS_OFF_INDEX(keyBegin,keyEnd)) { 
                ERROR_LOG   _jmap.clear();  return false; 
            }  
            if(IS_WAIT_INDEX(keyBegin,keyEnd) || IS_WAIT_INDEX(valBegin,valEnd)){ 
                continue; 
            }

            // [3.2] '}'Ϊ����'}'����Ϊjson�ṹ�ı߽磬ֱ�Ӽ�����Ⱥ������һ��ѭ��
            if(currentDeepness==1){ --currentDeepness; continue; }

            // [3.3] �п��ܵ�ǰ������һ�Լ�ֵ��βû��','������Ҫ��'}'�ж���û�д��ڿ��ܵ�key��value
            if(IS_ON_INDEX(keyBegin, keyEnd)) {
                if(!IS_ON_INDEX(valBegin, valEnd)){
                    // ���п��õ�keyʱ��value�������ǲ������﷨
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
            // [4.1] �ַ�'['��־������Ŀ�ʼ����Ҫ����һ��arrayInfo�Ķ���ǰ����Ҫ��һ������key����û��keyʱ˵���������﷨����
            // if(!IS_ON_INDEX(keyBeginIndex, keyEndIndex)){
            //     ERROR_LOG   _jmap.clear();  return false;
            // }

            // [4.2] ��ʱ���浽arrayPathToCount�У����ռ��ö�Ӧ������item�������������ֶ�Ӧ']'ʱ�ٻ�дarrayInfo����
            workingPath << fromTransferred(data+keyBegin, keyEnd-keyBegin);
            arrayPathToCount[workingPath] = 0;

            ++currentDeepness;
            isNeedResetIndex = true;
        }
        else if(']' == *offsetData){
            // [5.1] �ַ�']'��־������Ľ������п��ܵ�ǰ������һ�Լ�ֵ��βû��','������Ҫ��']'�ж���û�д��ڿ��ܵ�key��value
            std::string itemValue = "";
            if(IS_ON_INDEX(valBegin, valEnd)){
                itemValue = fromTransferred(data+valBegin, valEnd-valBegin);
            }

            // [5.2] ���һ��·����û�ж�Ӧ����Ϣ������Ϣ
            auto iter = arrayPathToCount.find(workingPath.data());
            if(arrayPathToCount.end() == iter){ 
                ERROR_LOG   _jmap.clear();  return false; 
            }

            // [5.3] д��ֵ
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
            // [6.1] ','Ϊkey��value�е��ַ���ֱ�ӽ�����һ��ѭ��
            if(IS_OFF_INDEX(keyBegin,keyEnd) || IS_WAIT_INDEX(keyBegin,keyEnd) || IS_WAIT_INDEX(valBegin,valEnd)){ continue; }

            // [6.2] ��ȡ��Ӧ��key��value
            std::string key     = IS_ON_INDEX(keyBegin,keyEnd) ? fromTransferred(data+keyBegin, keyEnd-keyBegin) : "";
            std::string value   = IS_ON_INDEX(valBegin,valEnd) ? fromTransferred(data+valBegin, valEnd-valBegin) : "";

            // [6.3] ���ֻ��key���������Ϊ�������е����ݣ���Ҫ�жϸ�·����û�ж�Ӧ������Ԫ����������
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
             * ������key�е��ַ����������value���ַ���������value���ַ�ǰ���ǣ��Ѿ���һ��������key��
             * ���Ե�key������ʱ��ֻ��Ҫ����continue���ɣ�
             * ��key����ʱ������Ҫ�ж���valueStr����valueInt��
             * ���������ʱû��valueStr����ʼ'\"'��valueStrStartIndex<0������ΪvalueInt
            */
            if(IS_ON_INDEX keyBegin < 0 || keyEnd < 0)    
                continue;
            if(valBegin < 0 && valEnd < 0) {
                valueIntBeginIndex = offset;
                valueIntEndIndex = offset+1;
            }  
            else if(valueIntBeginIndex >= 0){
                valueIntEndIndex = offset+1;
            }    
            else{}
        }
        else if(':' == *offsetData){
            
        }
        else{

        }
    }

    for (auto iter = arrayPathToCount.begin(); iter != arrayPathToCount.end(); iter++){
        _jmap[iter->first] = XXjvalue(XXstdStringExtend::count(iter->first, '/')+1, XXjvalue::Type::ArrayInfo).toData();   
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
std::string XXjson::fromTransferred(std::string &str, const char *data, int length){
    if(length <= 0){
        return;
    }
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

    // ��Ӽ�ֵ�ԣ�"key":"value", ���� "key":
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

// ��β������
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