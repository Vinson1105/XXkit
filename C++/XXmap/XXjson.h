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

    void fromMap(const XXmapRef ref);   /** ��ָ����XXmapת��Ϊ���صĸ�ʽ���� */  
    void toMap(XXmapRef ref);           // ������JMapת��ָ��PaMap

private:
    void printTabSpacer(std::string &str, uint8_t newDeepness);
    std::string replace(const std::string &src);

private:
    XXmap _jmap;
    std::string _lastTabSpacer;  //�ڴ�ӡʱ,��¼ǰһ���
};

#endif //XXjson_h
