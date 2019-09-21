/*
_* ����: JSon
_* �汾: v1.0  GuoWenXuan  2016-04-21;
_*       v1.1  GuoWenXuan  2016-04-22;
_*       1. ��ӡ'{'֮ǰ,�Ȼ���
_*       2. �Ż������Ĵ�ӡ
_*       3. JObject�Ľ�����֮ǰ����Ƚ�,��Ϊ��ǰ�Ƚ�,����¼ǰһ���
_*       4. �ؼ���ֻ���Ǳ�ʶ��(��Сд��ĸ,����,�»���), ֵ�в���ʹ�� ',' '{' '}'�ַ�
_*       v1.2 Lgzhi 2016-07-12
_*       1. ���Ӻ�����������
_*       2. PaMap��ص��������·��(��Ҫ�� ��Ե�, ��·��)
_*       v1.3 GuoWenXuan 2016-09-20
_*       1. �޸� fromPaMap �� toPaMap ���β�,���ǲ�Ӱ��ʹ��
_*       2. Value �Ľ�ȡ��ʽ��ԭ����section(':', -1) ����Ϊ section(':', 1), ������Value��ʹ�� ':'
_*       v1.4 Lgzhi 2017-04-25
_*       1. �ļ��е�JSON���� // ��Ϊע�ͷ�;
_*/

/*
˵��:
    1.key �� value �в��ܰ������·���: ˫����" ������{} ð��: ����, �ո�
    2.�����Ҫ ð��: ����, �ո� ��, ���԰� key �� value ��˫����������;
    3."/" ��ʾ Ŀ¼��ǰֵ, ������ΪĿ¼����, Ҳ������Ϊ���յ�key·���ڵ�, key�в��ܰ���'/';
    4.JSON�ļ�, ������ { ��ʼ, }��β, ���Ҹÿ�ʼ{�ͽ�β}Ҫ��һ��;
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

    QStringList list = jsonString.split('\r', QString::SkipEmptyParts); // �� '\r' �����jsonString

    QString workingKey;
    workingKey.clear();
    U8 workingDeepness = 0;

    QString key;
    QString value;
    for (QStringList::const_iterator constIter = list.constBegin(); constIter != list.constEnd(); constIter++)
    {
        if ("{" == (*constIter))
        {
            if (!workingKey.isEmpty() || workingDeepness > 1) //����ȱ��������Χ���� �� ����Χ���ŵ���һ��·��Ϊ����������
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
                workingKey.resize(workingKey.lastIndexOf('/')); //��resize()��Ŀ���Ǽӿ������ٶ�
            }
        }
        // ��Ŀ¼
        else if ('\t' == *((*constIter).constEnd()-1)) 
        {
            key = (*constIter).section('\t', 0, 0);
            if ("\"\"" == key)
                key = "";

            value.clear();
        }
        // ���ļ�
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
                    key.clear(); //���key�����䴥��{��дֵ
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

        // JsonObject�����ж�
        if (lastDeepness > currDeepness)
        {
            U8 dDeepness = lastDeepness - currDeepness;
            for (U8 rowCount = 0; rowCount < dDeepness; rowCount++)
            {
                printTabSpacer(textStream, lastDeepness - rowCount - 1);
                textStream << "},\r\n";
            }
        }

        // ����
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
        textStream << "\r\n"; // ����
        lastDeepness = currDeepness;
    }

    //  ����'}'
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

        // JsonObject�����ж�
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

    //  ����'}'
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

            // �Ƚ��ļ�����
            JValue newValue(deepness, 'F', iterM.value());
            jmap.insert(workingKey, newValue.toData());

            deepness--;
            workingKey.resize(workingKey.lastIndexOf('/'));

            // ��ȡ����Key,ÿ��ѭ��ɾ������"/+"�ڵ�
            for (; deepness > 0; deepness--, workingKey.resize(workingKey.lastIndexOf('/')))
            {
                // JMap�а������·��
                QMap<QString, QString>::iterator workingIter = jmap.find(workingKey);
                if (workingIter != jmap.end())
                {
                    JValue jvalue(workingIter.value());
                    // ���·���µ���һ���ļ�
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

// ����滻�㷨���ܷ�ֹ " ��������;
// ��� "" ����Ϊ "";
// ��� "..." ����Ķ�����������, �����治���� ", ת�� \" Ҳ����;
// ��� ' '�����¿����ַ�ȥ��, ��'\n'�ỻ��'\r', '\r'��ȥ��;
// ���� { �� } ���� + \r;
// ��� , �滻Ϊ \r;
// ��� : �滻Ϊ \t;
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
            if (kCopy && (i < src.length()) && ('\"' == src[i])) //ǰ��������Ǳ��� ���� '\"'ʱ, ���� '\"'ʱ�ַ���
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
                    result.append("\r");        //��"\n"����"\r", �����к���û��','�Ĵ���;
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
                    result.append("\r}\r");     //���� ��'\r', ���Լ��ݺ���û��','�Ĵ���;
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
