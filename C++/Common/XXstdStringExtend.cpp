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

std::string XXstdStringExtend::section(const std::string &target, const std::string &sep, uint8_t start, int8_t end){
    std::string sectionString;
    sectionString.reserve(64);

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