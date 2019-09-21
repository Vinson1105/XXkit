/*
_* 描述: JMap  JSon数据表
_* 版本: v1.0  GuoWenXuan  2016-04-21;
_*       1.toData()/fromData()函数将deepness数值HEX强制转到U8/U8转到HEX;
_*/

#include "JMap.h"

JValue::JValue(U8 deepness, char type, const QString &value)
{
    deepness_ = '0' + deepness;
    type_ = type;
    value_ = value;
}
JValue::JValue(const QString &jvalueString)
{
    if (!jvalueString.isEmpty())
        fromData(jvalueString);
}

char JValue::type() const
{
    return type_;
}
void JValue::type(char type)
{
    type_ = type;
}

U8 JValue::deepness() const
{
    return deepness_ - '0';
}
void JValue::deepness(U8 deepness)
{
    deepness_ = '0' + deepness;
}

QString JValue::value() const
{
    return value_;
}
void JValue::value(const QString &value)
{
    value_ = value;
}

QString JValue::toData() const
{
    return  QString((char)deepness_) + '/'+ type_ + '/' + value_;
}
JValue& JValue::fromData(const QString &string)
{
   std::string stdString = string.toStdString();
   if ('/' == stdString[1] && '/' == stdString[3])
    {
       deepness_ = stdString[0];
       type_ = stdString[2];
       value_ = QString(stdString.c_str()+4);
    }
    return *this;
}

void JValue::operator=(const JValue &jvalue)
{
    deepness_ = jvalue.deepness();
    type_ = jvalue.type();
    value_ = jvalue.value();
}
