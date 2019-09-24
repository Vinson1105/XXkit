#ifndef XXjson_h
#define XXjson_h

#include <string>
#include "XXmap.h"

class XXjson{
public:
    // static bool toFile(const std::string &path, XXmapRef ref, bool isThin = false);
    // static std::string toString(XXmapRef ref, bool isThin = false);
    // static bool toMapFromFile(XXmapRef ref, const std::string &path);
    // static bool toMapFromString(XXmapRef ref, const std::string &str);

public:
    XXjson(){}
    virtual ~XXjson(){}

    // bool fromFile(const std::string &path);
    // bool toFile(const std::string &path, Mode mode = J);

    // bool fromString(const std::string &json);
    // bool toString(std::string &jsonString, bool isThin);
    // bool toThinString(std::string &jsonString);

    void fromMap(XXmapRef ref);   /** 将指定的XXmap转换为本地的格式数据 */  
    //void toMap(XXmapRef ref);           // 将本地JMap转到指定PaMap

    std::string toString(bool isThin = false);

    std::map<std::string,std::string>& data() { return _jmap; }

private:
    void addPair(std::string &str, const std::string &key, const std::string &value = "");
    void addNewline(std::string &str);
    void addTabSpacer(std::string &str, uint8_t deepness);
    // void printTabSpacer(std::string &str, uint8_t newDeepness);
    // std::string replace(const std::string &src);

private:
    std::map<std::string,std::string> _jmap;
    std::string _lastTabSpacer;  //在打印时,记录前一深度
};

#endif //XXjson_h
