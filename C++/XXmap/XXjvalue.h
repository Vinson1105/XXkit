#ifndef XXjvalue_h
#define XXjvalue_h

#include <string>

class XXjvalue
{
public:
    enum Type{
        PathNode    = '{',  // 路径中间节点类型（没有对应的值）
        PathValue   = ':',  // 一般值类型
        ArrayInfo   = '[',  // 路径中间节点类型（没有对应的值，但该路径是数组的开始）
        ArrayItem   = '.',  // 路径中间节点类型（没有对应的值，但该路径是数组的item）
        ArrayValue  = '=',  // 数组值类型
    };

    XXjvalue(){}
    XXjvalue(int deepness, Type type, const std::string &value = "");
    XXjvalue(const std::string &jvalueString);
    virtual ~XXjvalue(){}

public: 
    uint8_t deepness() const { return _deepness; }
    Type type() const { return _type; }
    std::string& value() { return _value; }
    const std::string& value() const { return _value; }

public: 
    void deepness(uint8_t deepness) {_deepness = deepness; }
    void type(Type type) { _type = type;}
    void value(const std::string &value) { _value = value; }

public:
    std::string toData() const;
    XXjvalue& fromData(const std::string &jvalueString);
    void operator=(const XXjvalue &jvalue);

private:
    uint8_t _deepness;
    Type _type;
    std::string _value;
};

#endif
