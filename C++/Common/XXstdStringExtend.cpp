#include "XXstdStringExtend.h"

using namespace std;
std::vector<StringShared> XXstdStringExtend::split(const std::string &target, const std::string &sep){
    vector<StringShared> stringVector;
    stringVector.reserve(32);

    int seqLength       = sep.length();
    int targetLength    = target.length();
    int nodeBeginPos    = 0;  

    for(int pos = 0; pos < targetLength - seqLength; pos++)
    {
        if(0 == target.compare(pos, seqLength, sep)){
            if(nodeBeginPos == pos){
                stringVector.push_back(StringShared(new string("")));
            }
            else{
                string node = target.substr(nodeBeginPos, pos - nodeBeginPos);
                stringVector.push_back(StringShared(new string(node)));
            }
            nodeBeginPos = pos + seqLength;
        }
    }

    return stringVector;
}

std::string XXstdStringExtend::section(const std::string &target, const std::string &sep, uint8_t start, int8_t end){
    std::string sectionString;
    sectionString.reserve(64);

    auto stringVector =  XXstdStringExtend::split(target, sep);
    int size = stringVector.size();
    bool hasBegin = false;
    for (int index = 0; index < size; index++)
    {
        if (start == index){
            sectionString.append(*stringVector[index]);
            hasBegin = true;
        }
        else if(hasBegin){
            sectionString.append(sep);
            sectionString.append(*stringVector[index]);
        }
        else{

        }
    }

    return sectionString;
}