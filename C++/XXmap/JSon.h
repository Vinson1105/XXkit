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
_*/

#ifndef __JSon_h
#define __JSon_h

#include <QString>
#include <QTextStream>
#include "JMap.h"

class JSon
{
public:
    // �������Ϻ���
    static Fi loadToPaMap(PaMapRef ref, const QString &file);
    static void saveFromPaMap(const QString &file, PaMapRef ref);

	static QString pamapToString(PaMapRef ref);     // �������QT����, QStringֱ�����
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
        // Thin: ThinString, ֻ��toFile()��Ч;
            Dir:"Value1",{Key = "Value2",},
        */

    Fi fromFile(const QString &path);
    Fi toFile(const QString &path, Mode mode = J);

    Fi fromString(const QString &json);
    Fi toString(QString &oJsonString, Mode mode = J);
    Fi toThinString(QString &oJsonString);  //����ʡ�ռ��Kģʽ�ַ���,ȥ�����пո�ͻ���

    void fromPaMap(const PaMapRef paMapRef, const QString &relPath = "");   // ��ָ����PaMapת�ɱ���JMap
    void toPaMap(PaMapRef paMapRef, const QString &relPath = "");           // ������JMapת��ָ��PaMap

    JMap& jamp();

private:
    void printTabSpacer(QTextStream &oTextStream, U8 newDeepness);
    QString replace(const QString &src);

private:
    JMap jmap_;
    QString lastTabSpacer_;  //�ڴ�ӡʱ,��¼ǰһ���
};

#endif //__JSon_h
