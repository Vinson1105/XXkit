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
    void toMap(XXmapRef ref);     /** 将本地JMap转到指定XXmap */

    std::string toString(bool usingThin = false, bool usingTransferred = false, unsigned int kvSizeMax = 512);
    bool fromString(const std::string &jsonString);

    std::map<std::string,std::string>& data() { return _jmap; }

private:
    void toTransferred(char *buffer, int bufferSize, const std::string &str);   /** 转义：将\和"转换到\\和\"（\后一字节为x或者[0-9]不作转换） */
    std::string fromTransferred(const char *data, int length);                  /** 去转义：将\\和\"转换到\和" \后一字节为x或者[0-9]不作转换）*/
    void addPair(std::string &str, const std::string &key, const std::string &value = "");
    void addTabSpacer(std::string &str, uint8_t deepness);

private:
    std::map<std::string,std::string> _jmap;
    std::string _lastTabSpacer;  //在打印时,记录前一深度
};

#endif //XXjson_h
