/*
_* 描述: JSon
_* 版本: v1.0  GuoWenXuan  2016-04-21;
_*       v1.1  GuoWenXuan  2016-04-22;
_*       1. 打印'{'之前,先换行
_*       2. 优化缩进的打印
_*       3. JObject的结束从之前往后比较,改为往前比较,并记录前一深度
_*       4. 关键词只能是标识符(大小写字母,数字,下划线), 值中不能使用 ',' '{' '}'字符
_*       v1.2 Lgzhi 2016-07-12
_*       1. 增加函数保护代码
_*       2. PaMap相关的增加相对路径(主要是 相对的, 是路径)
_*       v1.3 GuoWenXuan 2016-09-20
_*       1. 修改 fromPaMap 和 toPaMap 的形参,但是不影响使用
_*/

#ifndef XXjson_h
#define XXjson_h

#include <string>
#include "XXmap.h"

class XXjson{
public:
    static bool toFile(const std::string &path, XXmapRef ref, bool isThin = false);
    static std::string toString(XXmapRef ref, bool isThin = false);
    static bool toMapFromFile(XXmapRef ref, const std::string &path);
    static bool toMapFromString(XXmapRef ref, const std::string &str);

public:
    XXjson(){}
    virtual ~XXjson(){}

    bool fromFile(const std::string &path);
    bool toFile(const std::string &path, Mode mode = J);

    bool fromString(const std::string &json);
    bool toString(std::string &jsonString, bool isThin);
    bool toThinString(std::string &jsonString);

    void fromMap(const XXmapRef ref);   /** 将指定的XXmap转换为本地的格式数据 */  
    void toMap(XXmapRef ref);           // 将本地JMap转到指定PaMap

private:
    void printTabSpacer(std::string &str, uint8_t newDeepness);
    std::string replace(const std::string &src);

private:
    XXmap _jmap;
    std::string _lastTabSpacer;  //在打印时,记录前一深度
};

#endif //XXjson_h
