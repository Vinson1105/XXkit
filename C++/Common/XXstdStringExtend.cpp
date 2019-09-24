#include "XXstdStringExtend.h"

using namespace std;
std::vector<std::string> XXstdStringExtend::splitToVector(const std::string &target, const std::string &sep){
    vector<string> stringVector;
    stringVector.reserve(32);

    int seqLength       = sep.length();
    int targetLength    = target.length();
    int nodeBeginPos    = 0;  

    for(int pos = 0; pos < targetLength - seqLength; pos++)
    {
        if(0 == target.compare(pos, seqLength, sep)){
            if(nodeBeginPos == pos){
                stringVector.push_back("");
            }
            else{
                string node = target.substr(nodeBeginPos, pos - nodeBeginPos);
                stringVector.push_back(node);
            }
            nodeBeginPos = pos + seqLength;
        }
    }

    stringVector.push_back(target.substr(nodeBeginPos));
    return stringVector;
}
std::vector<std::string> XXstdStringExtend::splitToVector(const std::string &target, const char *sep){
    return splitToVector(target, string(sep));
}
std::vector<std::string> XXstdStringExtend::splitToVector(const std::string &target, char sep){
    return splitToVector(target, string(1, sep));
}

std::list<std::string> XXstdStringExtend::splitToList(const std::string &target, const std::string &sep){
    list<string> stringList;

    int seqLength       = sep.length();
    int targetLength    = target.length();
    int nodeBeginPos    = 0; 

    for(int pos = 0; pos < targetLength - seqLength; pos++)
    {
        if(0 == target.compare(pos, seqLength, sep)){
            if(nodeBeginPos == pos){
                stringList.push_back("");
            }
            else{
                string node = target.substr(nodeBeginPos, pos - nodeBeginPos);
                stringList.push_back(node);
            }
            nodeBeginPos = pos + seqLength;
        }
    }

    stringList.push_back(target.substr(nodeBeginPos));
    return stringList;
}
std::list<std::string> XXstdStringExtend::splitToList(const std::string &target, const char *sep){
    return splitToList(target, string(sep));
}
std::list<std::string> XXstdStringExtend::splitToList(const std::string &target, char sep){
    return splitToList(target, string(1, sep));
}

std::string XXstdStringExtend::section(const std::string &target, const std::string &sep, int8_t start, int8_t end){
    int targetSize = target.size();
    int sepSize = sep.size();
    if(0 == targetSize || 0 == sepSize || start < end){
        return "";
    }

    std::string sectionString;
    sectionString.reserve(64);

    auto indexs = indexVector(target, sep);
    int startIndex;
    auto iter; //indexs.cbegin() + start : indexs.crbegin()+ abs(start-1);
    if(start > 0){
        iter = (indexs.begin()+start);
    }
    else{
        iter = (indexs.rbegin()+abs(start-1));
    }
    
    if(0 == start) {
        startIndex = 0;
    }         
    else if(start > 0){
        startIndex = indexOf(target, sep);
    }
    else if(-1 == start){
        startIndex = 0 == indexs.size() ? 0 : indexs.back()+sepSize;
    }
    else{
        //auto iter   = indexs.crbegin()+ abs(start-1);
        startIndex  = indexs.crend() == iter ? 0 : *iter+sepSize;
    }

    int endIndex;
    if(0 == end){
        endIndex = 0;
    }

    auto stringVector =  XXstdStringExtend::splitToVector(target, sep);
    int size = stringVector.size();
    bool hasBegin = false;
    for (int index = 0; index < size; index++)
    {
        if (start == index){
            sectionString.append(stringVector[index]);
            hasBegin = true;
        }
        else if(hasBegin){
            sectionString.append(sep);
            sectionString.append(stringVector[index]);
        }
        else{

        }
    }

    return sectionString;
}

bool XXstdStringExtend::hasPrefix(const std::string &target, const std::string &prefix){
    if(0 == target.size() || 0 == prefix.size() || prefix.size() > target.size()){
        return false;
    }
    return 0 == memcmp(target.data(), prefix.data(), prefix.size());
}
bool XXstdStringExtend::hasSuffix(const std::string &target, const std::string &suffix){
    if(0 == target.size() || 0 == suffix.size() || suffix.size() > target.size()){
        return false;
    }
    int targetOffset = target.size() - suffix.size();
    return 0 == memcmp(target.data()+targetOffset, suffix.data(), suffix.size());
}
int XXstdStringExtend::count(const std::string &target, const char c){
    int countOfC = 0;
    for (auto iter = target.cbegin(); iter != target.cend(); iter++){
       if (c == *iter){ ++countOfC; }
    }
    return countOfC;
}
int XXstdStringExtend::count(const std::string &target, const std::string &sep){
    int sepSize         = sep.size();
    int targetSize      = target.size();
    if(0 == targetSize || 0 == sepSize){
        return 0;
    }

    int countOfSep = 0;
    const char *targetPtr = target.data();
    const char *sepPtr = sep.data();
    for (int offset = 0; offset < targetSize;){
        if(0 == memcmp(targetPtr + offset, sepPtr, sepSize)){
            offset += sepSize;
            ++countOfSep;
        }
        else{
            offset++;
        }
    }
    return countOfSep;
}


std::vector<int> XXstdStringExtend::indexVector(const std::string &target, const std::string sep){
    std::vector<int> indexVector;
    int sepSize         = sep.size();
    int targetSize      = target.size();
    if(0 == targetSize || 0 == sepSize){
        return indexVector;
    }

    const char *targetPtr = target.data();
    const char *sepPtr = sep.data();
    for (int offset = 0; offset < targetSize;){
        if(0 == memcmp(targetPtr + offset, sepPtr, sepSize)){
            indexVector.push_back(offset);
            offset += sepSize;
        }
        else{
            offset++;
        }
    }
    return indexVector;
}
int XXstdStringExtend::indexOf(const std::string &target, const char c, int index){
    if(0 == target.size()){
        return -1;
    }
    int ret = 0;
    for (auto iter = target.cbegin(); iter != target.cend(); iter++,ret++){
        if (c != *iter)     { continue; }
        if (--index < 0)    { return ret; }
    }
    return -1;
}
int XXstdStringExtend::indexOf(const std::string &target, const std::string sep, int index){
    int sepSize         = sep.size();
    int targetSize      = target.size();
    if(0 == targetSize || 0 == sepSize){
        return -1;
    }

    const char *targetPtr = target.data();
    const char *sepPtr = sep.data();
    for (int offset = 0; offset < targetSize;){
        if(0 == memcmp(targetPtr + offset, sepPtr, sepSize)){
            if(--index < 0) return offset;
            offset += sepSize;
        }
        else{
            offset++;
        }
    }
    return -1;
}
int XXstdStringExtend::lastIndexOf(const std::string &target, const char c, int index){
    if(0 == target.size()){
        return -1;
    }
    int ret = target.size();
    for (auto iter = target.crbegin(); iter != target.crend(); iter++,ret--){
        if (c != *iter)     { continue; }
        if (--index < 0)    { return ret; }
    }
    return -1;
}
 int XXstdStringExtend::lastIndexOf(const std::string &target, const std::string sep, int index){
    int sepSize         = sep.size();
    int targetSize      = target.size();
    if(0 == targetSize || 0 == sepSize){
        return -1;
    }

    const char *targetPtr   = target.data();
    const char *sepPtr      = sep.data();
    for (int offset = targetSize - sepSize; offset >= 0;){
        if(0 == memcmp(targetPtr, sepPtr, sepSize)){
            if(--index < 0) return offset;
            offset -= sepSize;
        }
        else{
            offset--;
        }
    }
    return -1;
}