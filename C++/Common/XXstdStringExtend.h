#ifndef XXstdStringExtend_h
#define XXstdStringExtend_h

#include <string>
#include <list>
#include <vector>
#include <memory>

typedef std::shared_ptr<std::string> StringShared;
namespace XXstdStringExtend{
    /**
     * @brief   字符串分割，按照指定的标识进行分割，会保存空字符串节点（“”）
     * @param   target    需要分割的目标字符串
     * @param   sep       分割的字符串标识
     * @return  返回已经分割的字符串节点数组
    */
    std::vector<StringShared> split(const std::string &target, const std::string &sep);

    std::string section(const std::string &target, const std::string &sep, uint8_t start, int8_t end);
};
#endif
