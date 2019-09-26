#ifndef XXjvalue_h
#define XXjvalue_h

#include <string>

class XXjvalue
{
public:
    enum Type{
        PathNode    = '{',  // ??????????????
        PathValue   = ':',  // ???????????????
        ArrayInfo   = '[',  // ?????????????????????arrayInfo
        ArrayItem   = '.',  // ?????????????????????????
        ArrayValue  = '=',  // ???????????????????Array/0 = Array0 
    };

    XXjvalue(){}
    XXjvalue(int deepness, Type type, const std::string &value = "");
    XXjvalue(const std::string &jvalueString);
    virtual ~XXjvalue(){}

public: /** Getter */
    uint8_t deepness() const { return _deepness; }
    Type type() const { return _type; }
    std::string value() const { return _value; }

public: /** Setter */
    void deepness(uint8_t deepness) {_deepness = deepness; }
    void type(Type type) { _type = type;}
    void value(const std::string &value) { _value = value; }

public: /** ???? */
    std::string toData() const;
    XXjvalue& fromData(const std::string &jvalueString);
    void operator=(const XXjvalue &jvalue);

private:
    uint8_t _deepness;
    Type _type;
    std::string _value;
};

#endif
