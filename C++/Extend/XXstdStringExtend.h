#ifndef XXstdStringExtend_h
#define XXstdStringExtend_h

#include <string>
#include <list>
#include <vector>
#include <memory>

namespace XXstdStringExtend{
    /**
     * @brief   字符串分割，按照指定的标识进行分割，会保存空字符串节点（“”）
     * @param   target  需要分割的目标字符串
     * @param   sep     分割的字符串标识
     * @return  返回已经分割的字符串节点vector
    */
    std::vector<std::string> splitToVector(const std::string &target, const std::string &sep);
    std::vector<std::string> splitToVector(const std::string &target, const char *sep);
    std::vector<std::string> splitToVector(const std::string &target, char sep);

    /**
     * @brief   字符串分割，按照指定的标识进行分割，会保存空字符串节点（“”）
     * @param   target  需要分割的目标字符串
     * @param   sep     分割的字符串标识
     * @return  返回已经分割的字符串节点list
    */
    std::list<std::string> splitToList(const std::string &target, const std::string &sep);
    std::list<std::string> splitToList(const std::string &target, const char *sep);
    std::list<std::string> splitToList(const std::string &target, char sep);

    /**
     * @brief   字符串提取，按照指定的标识进行提取
     * @param   target  需要提取的目标字符串
     * @param   sep     提取的字符串标识
     * @param   start   起始序号
     * @param   end     结束序号
     * @return  返回从起始序号到结束序号之后的字符串
    */
    std::string section(const std::string &target, const std::string &sep, int8_t start, int8_t end = -1);

    /** 判断目标字符串<target>是否已指定字符串<prefix>开始 */
    bool hasPrefix(const std::string &target, const std::string &prefix);
    /** 判断目标字符串<target>是否已指定字符串<suffix>结束 */
    bool hasSuffix(const std::string &target, const std::string &suffix);

    int count(const std::string &target, const char c);
    int count(const std::string &target, const std::string &sep);

    std::vector<int> indexVector(const std::string &target, const std::string sep);
    int indexOf(const std::string &target, const char c, int index = 0);
    int indexOf(const std::string &target, const std::string sep, int index = 0);
    int lastIndexOf(const std::string &target, const char c, int index = 0);
    int lastIndexOf(const std::string &target, const std::string sep, int index = 0);
};
#endif
