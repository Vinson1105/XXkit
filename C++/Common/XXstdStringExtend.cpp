#include "XXstdStringExtend.h"

vector<StringSharded> split(const string &target, const string &sep){
    vector<StringSharded> stringVector;
    stringVector.reserve(20);

    int seqLength       = sep.length();
    int targetLength    = target.length();
    int nodeBeginPos    = 0;  

    for(int pos = 0; pos < targetLength - seqLength; pos++)
    {
        if(0 == target.compare(pos, seqLength, sep)){
            if(nodeBeginPos == pos){
                stringVector.push_back(StringSharded(new string("")));
            }
            else{
                string node = target.substr(nodeBeginPos, pos - nodeBeginPos);
                stringVector.push_back(StringSharded(new string(node)));
            }
            nodeBeginPos = pos;
        }
    }

    return stringVector;
}
