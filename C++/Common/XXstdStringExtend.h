#ifndef XXstdStringExtend_h
#define XXstdStringExtend_h

#include <string>
#include <list>
#include <vector>
#include <memory>

using namespace std;
typedef std::shared_ptr<std::string> StringSharded;

namespace XXstdStringExtend{
    /**
     * @brief   字符串分割，按照指定的标识进行分割，会保存空字符串节点（“”）
     * @param   target    需要分割的目标字符串
     * @param   sep       分割的字符串标识
     * @return  返回已经分割的字符串节点数组
    */
    vector<StringSharded> split(const string &target, const string &sep);
};


#endif