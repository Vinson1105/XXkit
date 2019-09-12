#ifndef XXjson_h
#define XXjson_h

#include <string>
#include "XXdataBase.h"

namespace XXjson{
    std::string toString(const XXdataBaseRef dataBaseRef);
    bool toFile(const XXdataBaseRef dataBaseRef, const std::string &path);
    bool fromString(XXdataBase &dataBaseRef, const std::string &string);
    bool fromFile(XXdataBase &dataBaseRef, const std::string &path);
};

#endif