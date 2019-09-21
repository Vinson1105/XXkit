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
_*       2. Value 的截取方式从原来的section(':', -1) 更改为 section(':', 1), 可以在Value中使用 ':'
_*       v1.4 Lgzhi 2017-04-25
_*       1. 文件中的JSON增加 // 作为注释符;
_*/

/*
说明:
    1.key 和 value 中不能包含以下符号: 双引号" 大括号{} 冒号: 逗号, 空格
    2.如果需要 冒号: 逗号, 空格 的, 可以把 key 或 value 用双引号括起来;
    3."/" 表示 目录当前值, 不能作为目录名称, 也不能作为多层空的key路径节点, key中不能包含'/';
    4.JSON文件, 必须以 { 开始, }结尾, 并且该开始{和结尾}要成一对;
*/

#include "Json.h"
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>

Fi JSon::loadToPaMap(PaMapRef ref, const QString &file)
{
    JSon json;
    if (json.fromFile(file))
    {
        json.toPaMap(ref);
        return 1;
    }
    return 0;
}
void JSon::saveFromPaMap(const QString &file, PaMapRef ref)
{
    JSon json;
    json.fromPaMap(ref);
    json.toFile(file, JSon::Pa);
}

QString JSon::pamapToString(PaMapRef ref)
{
	JSon json;
    QString s;
	json.fromPaMap(ref);
	json.toString(s);
	return s;
}
QString JSon::pamapToThinString(PaMapRef ref)
{
    JSon json;
    QString s;
    json.fromPaMap(ref);
    json.toThinString(s);
    return s;
}

PaMap JSon::stringToPaMap(const QString &str)
{
	JSon json;
    PaMap map;
	json.fromString(str);
	json.toPaMap(PaMapRef(map));
	return map;
}
void JSon::stringToPaMap(PaMapRef ref, const QString &str)
{
    JSon json;
    json.fromString(str);
    json.toPaMap(ref);
}

Fi JSon::fromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return 0;

    QString fileStirng = QString::fromLocal8Bit(file.readAll());
    file.close();

    fileStirng.replace(QRegExp("\\s*//[^\n]*\n*"), "\r\n");
    return fromString(fileStirng);
}
Fi JSon::toFile(const QString &path, Mode mode)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadWrite))
    {
        QFileInfo fileInfo(path);
        QDir dir;
        if (!dir.mkpath(fileInfo.path()))
            return 0;
        file.open(QIODevice::ReadWrite);
    }

    file.resize(0);

    QString jsonString;
    QTextStream textStream(&file);

    Fi ret;
    if (Thin == mode)
        ret = toThinString(jsonString);
    else
        ret = toString(jsonString, mode);
    textStream << jsonString;

    file.close();
    return ret;
}

Fi JSon::fromString(const QString &json)
{
    if (json.isEmpty())
        return 0;

    QString jsonString = replace(json);
    QMap<QString, QString> &jmap = jmap_.data();
    jmap.clear();

    QStringList list = jsonString.split('\r', QString::SkipEmptyParts); // 以 '\r' 来拆分jsonString

    QString workingKey;
    workingKey.clear();
    U8 workingDeepness = 0;

    QString key;
    QString value;
    for (QStringList::const_iterator constIter = list.constBegin(); constIter != list.constEnd(); constIter++)
    {
        if ("{" == (*constIter))
        {
            if (!workingKey.isEmpty() || workingDeepness > 1) //兼容缺少最大的外围括号 和 有外围括号但第一个路径为空两大问题
                workingKey += '/';

            if (!key.isEmpty())
            {
                workingKey += key;

                JValue jvalue(workingDeepness, 'D', value);
                jmap.insert(workingKey, jvalue.toData());
            }

            workingDeepness++;
            key.clear();
            value.clear();
        }
        else if ("}" == (*constIter))
        {
            if (workingDeepness > 0)
            {
                workingDeepness--;
                workingKey.resize(workingKey.lastIndexOf('/')); //用resize()的目的是加快运算速度
            }
        }
        // 是目录
        else if ('\t' == *((*constIter).constEnd()-1)) 
        {
            key = (*constIter).section('\t', 0, 0);
            if ("\"\"" == key)
                key = "";

            value.clear();
        }
        // 是文件
        else
        {
            key = (*constIter).section('\t', 0, 0);
            value = (*constIter).section('\t', 1);

            if ("\"\"" == key)
                key = "";
            if ("\"\"" == value)
                value = "";

            if (!value.isEmpty())
            {
                if ("/" == key || key.isEmpty())
                {
                    JValue jvalue(workingDeepness, 'D', value);
                    jmap.insert(workingKey, jvalue.toData());
                    key.clear(); //清空key不让其触发{的写值
                }
                else
                {
                    JValue jvalue(workingDeepness, 'F', value);
                    if (workingKey.isEmpty())
                        jmap.insert(key, jvalue.toData());
                    else
                        jmap.insert(workingKey + '/' + key, jvalue.toData());
                }
            }
        }
    }
    return 1;
}
Fi JSon::toString(QString &oJsonString, Mode mode)
{
    QMap<QString, QString> &map = jmap_.data();
    if (0 == map.size())
        return 0;

    QTextStream textStream(&oJsonString);
    uint currDeepness = 0;
    uint lastDeepness = 0;
    textStream << "{\r\n";

    for (QMap<QString, QString>::const_iterator mapIter = map.constBegin(); mapIter != map.constEnd(); mapIter++)
    {
        JValue jvalue(mapIter.value());
        currDeepness = jvalue.deepness();

        // JsonObject结束判断
        if (lastDeepness > currDeepness)
        {
            U8 dDeepness = lastDeepness - currDeepness;
            for (U8 rowCount = 0; rowCount < dDeepness; rowCount++)
            {
                printTabSpacer(textStream, lastDeepness - rowCount - 1);
                textStream << "},\r\n";
            }
        }

        // 缩进
        printTabSpacer(textStream, currDeepness);

        // Key(1: or "1":)
        QString key;
        if (J == mode)
            key = "\"" + mapIter.key().section('/', -1) + "\"";
        else
            key = mapIter.key().section('/', -1);

        // Value
        if ('F' == jvalue.type())
        {
            textStream << key << " : \"" << jvalue.value() << "\",";
        }
        else if ('D' == jvalue.type())
        {
            if (jvalue.value().isEmpty())
            {
                if (!key.isEmpty() && "\"\"" != key)
                {
                    textStream << key << " :\r\n";
                    printTabSpacer(textStream, currDeepness);
                }
                textStream << '{';
                currDeepness++;
            }
            else
            {
                if (Pa == mode)
                {
                    textStream << key << " : \"" << jvalue.value() << "\",\r\n";
                    printTabSpacer(textStream, currDeepness);
                    textStream << "{";
                    currDeepness++;
                }
                else
                {
                    textStream << key << " :\r\n";

                    printTabSpacer(textStream, currDeepness);
                    textStream << "{\r\n";
                    currDeepness++;

                    printTabSpacer(textStream, currDeepness);
                    if (J == mode)
                        textStream << "\"/\" : ";
                    else
                        textStream << "/ : ";
                    textStream << "\"" << jvalue.value() << "\",";
                }
            }
        }
        else
        {
            textStream << ',';
        }
        textStream << "\r\n"; // 换行
        lastDeepness = currDeepness;
    }

    //  最后的'}'
    if (lastDeepness)
    {
        --lastDeepness;
        for (U8 rowCount = 0; rowCount < lastDeepness; rowCount++)
        {
            printTabSpacer(textStream, lastDeepness - rowCount);
            textStream << "},\r\n";
        }
    }
    textStream << "}\r\n";
    return 1;
}

Fi JSon::toThinString(QString &oJsonString)
{
    QMap<QString, QString> &map = jmap_.data();
    if (0 == map.size())
        return 0;

    QString colon = ":";

    QTextStream textStream(&oJsonString);
    uint currDeepness = 0;
    uint lastDeepness = 0;
    textStream << "{";

    for (QMap<QString, QString>::const_iterator mapIter = map.constBegin(); mapIter != map.constEnd(); mapIter++)
    {
        JValue jvalue(mapIter.value());
        currDeepness = jvalue.deepness();

        // JsonObject结束判断
        if (lastDeepness > currDeepness)
        {
            U8 dDeepness = lastDeepness - currDeepness;
            for (U8 rowCount = 0; rowCount < dDeepness; rowCount++)
            {
                textStream << "}";
            }
        }

        // Key
        QString key = mapIter.key().section('/', -1);

        // Value
        if ('F' == jvalue.type())
        {
            textStream << key << colon << "\"" << jvalue.value() << "\",";
        }
        else if ('D' == jvalue.type())
        {
            if (jvalue.value().isEmpty())
            {
                if (!key.isEmpty() && "\"\"" != key)
                    textStream << key << colon;

                textStream << '{';
                currDeepness++;
            }
            else
            {
                textStream << key << colon << "\"" << jvalue.value() << "\",{";
                currDeepness++;
            }
        }
        else
        {
            textStream << ',';
        }
        lastDeepness = currDeepness;
    }

    //  最后的'}'
    if (lastDeepness)
    {
        --lastDeepness;
        for (U8 rowCount = 0; rowCount < lastDeepness; rowCount++)
        {
            textStream << "}";
        }
    }
    textStream << "}";
    return 1;
}

void JSon::fromPaMap(const PaMapRef paMapRef, const QString &relPath)
{
    PaMap paMap;
    paMapRef.load(relPath, paMap);
    const QMap<QString, QString> &dataMap = paMap.data();

    if (dataMap.size())
    {
        QMap<QString, QString> &jmap = jmap_.data();
        for (QMap<QString, QString>::const_iterator iterM = dataMap.constBegin(); iterM != dataMap.constEnd(); iterM++)
        {
            QString workingKey = iterM.key();
            U8 deepness = workingKey.count('/') + 1;

            // 先将文件处理
            JValue newValue(deepness, 'F', iterM.value());
            jmap.insert(workingKey, newValue.toData());

            deepness--;
            workingKey.resize(workingKey.lastIndexOf('/'));

            // 获取整个Key,每次循环删除最后的"/+"节点
            for (; deepness > 0; deepness--, workingKey.resize(workingKey.lastIndexOf('/')))
            {
                // JMap中包含这个路径
                QMap<QString, QString>::iterator workingIter = jmap.find(workingKey);
                if (workingIter != jmap.end())
                {
                    JValue jvalue(workingIter.value());
                    // 这个路径下的是一个文件
                    if ('F' == jvalue.type())
                    {
                        jvalue.type('D');
                        workingIter.value() = jvalue.toData();
                    }
                }
                else
                {
                    jmap.insert(workingKey, JValue(deepness, 'D').toData());
                }
            }
        }
    }
}
void JSon::toPaMap(PaMapRef paMapRef, const QString &relPath)
{
    QMap<QString, QString>& jmap = jmap_.data();
    QString path = relPath;
    if (!relPath.isEmpty() && '/' != relPath[relPath.size() - 1])
        path += '/';

    JValue jvalue;
    for (QMap<QString, QString>::iterator iter = jmap.begin(); iter != jmap.end(); iter++)
    {
        jvalue.fromData(iter.value());
        if (jvalue.value().isEmpty())
            continue;

        QString key = iter.key();
        paMapRef.set(path + key, jvalue.value());
    }
}

JMap& JSon::jamp()
{
    return jmap_;
}


void JSon::printTabSpacer(QTextStream &oTextStream, U8 deepness)
{
    lastTabSpacer_.resize(deepness * 4, ' ');
    oTextStream << lastTabSpacer_;
}

// 这个替换算法不能防止 " 数量出错;
// 会把 "" 保留为 "";
// 会把 "..." 里面的东西完整保留, 但里面不能有 ", 转意 \" 也不行;
// 会把 ' '及以下控制字符去掉, 但'\n'会换成'\r', '\r'会去掉;
// 会在 { 和 } 两边 + \r;
// 会把 , 替换为 \r;
// 会把 : 替换为 \t;
QString JSon::replace(const QString &src)
{
    bool kCopy = false;
    QString result;

    uint i = 0;
    while (i < src.length())
    {
        QChar ch = src[i];
        i++;
        if ('\"' == ch)
        {
            kCopy = !kCopy;
            if (kCopy && (i < src.length()) && ('\"' == src[i])) //前面的条件是避免 单数 '\"'时, 最后的 '\"'时字符串
            {
                i++;
                kCopy = false;
                result.append("\"\"");
            }
        }
        else
        {
            if (kCopy)
            {
                result.append(ch);
            }
            else
            {
                if ('\n' == ch)
                {
                    result.append("\r");        //把"\n"换成"\r", 兼容行后面没有','的错误;
                }
                else if (ch <= ' ')
                {
                }
                else if ('{' == ch)
                {
                    result.append("\r{\r");
                }
                else if ('}' == ch)
                {
                    result.append("\r}\r");     //后面 加'\r', 可以兼容后面没有','的错误;
                }
                else if (',' == ch)
                {
                    result.append("\r");
                }
                else if (':' == ch)
                {
                    result.append('\t');
                }
                else
                {
                    result.append(ch);
                }
            }
        }
    }
    return result;
}
