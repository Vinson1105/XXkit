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

#ifndef __JSon_h
#define __JSon_h

#include <QString>
#include <QTextStream>
#include "JMap.h"

class JSon
{
public:
    // 常用整合函数
    static Fi loadToPaMap(PaMapRef ref, const QString &file);
    static void saveFromPaMap(const QString &file, PaMapRef ref);

	static QString pamapToString(PaMapRef ref);     // 这里根据QT特性, QString直接输出
    static QString pamapToThinString(PaMapRef ref);

	static PaMap stringToPaMap(const QString &str);
    static void stringToPaMap(PaMapRef ref, const QString &str);

public:
    JSon()
    {}
    virtual ~JSon()
    {}

    enum Mode { J = 0, B = 1, Pa = 2, Thin = 3, };
        /* J:
            "Dir" :
            {
                "/" : "Value1",
                "Key" = "Value2", 
            },
        // B:
            Dir :
            {
                / : "Value1",
                Key = "Value2",
            },
        // Pa:
            Dir : "Value1",
            {
                Key = "Value2",
            },
        // Thin: ThinString, 只对toFile()有效;
            Dir:"Value1",{Key = "Value2",},
        */

    Fi fromFile(const QString &path);
    Fi toFile(const QString &path, Mode mode = J);

    Fi fromString(const QString &json);
    Fi toString(QString &oJsonString, Mode mode = J);
    Fi toThinString(QString &oJsonString);  //返回省空间的K模式字符串,去掉所有空格和换行

    void fromPaMap(const PaMapRef paMapRef, const QString &relPath = "");   // 将指定的PaMap转成本地JMap
    void toPaMap(PaMapRef paMapRef, const QString &relPath = "");           // 将本地JMap转到指定PaMap

    JMap& jamp();

private:
    void printTabSpacer(QTextStream &oTextStream, U8 newDeepness);
    QString replace(const QString &src);

private:
    JMap jmap_;
    QString lastTabSpacer_;  //在打印时,记录前一深度
};

#endif //__JSon_h
