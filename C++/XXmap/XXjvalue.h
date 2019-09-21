#ifndef XXjvalue_h
#define XXjvalue_h

#include <string>

class XXjvalue
{
public:
    enum Type{
        Path,
        Value,
        Array,
    };
    static XXjvalue create
    XXjvalue(int deepness, Type type, const std::string &value);
    XXjvalue(const std::string &jvalueString);
    virtual ~XXjvalue(){}

    Type type() const;
    void type(Type type);

    U8 deepness() const;
    void deepness(U8 deepness);

    QString value() const;
    void value(const QString &value);

    QString toData() const;
    JValue& fromData(const QString &string);

    void operator=(const JValue &jvalue);

private:
    char type_;
    QString value_;
    U8 deepness_;
};

#endif
