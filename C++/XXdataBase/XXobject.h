#ifndef XXobject_h
#define XXobject_h

#include <stdio.h>
#include <string>
#include <vector>

typedef enum XXobjectType{
    XXobjectType_Int            = 1,        
    XXobjectType_Double,
    XXobjectType_String,
    XXobjectType_Vector,
    XXobjectType_CustomBegin    = 100,
} XXobjectType;
class XXobject;
typedef std::vector< std::shared_ptr<XXobject> > XXvector;

class XXintObject;
class XXdoubleObject;
class XXstringObject;
class XXvectorObject;
class XXobject {
    friend class XXintObject;
    friend class XXdoubleObject;
    friend class XXstringObject;
    friend class XXvectorObject;
protected:
    XXobject(XXobjectType type) 
        : _type(type)
        , _intValue(0)
        , _doubleValue(0)
        , _stringValue("")
        , _vectorValue(XXvector()){}
    XXobjectType _type; 

private: 
    int _intValue;                  
    double _doubleValue;
    std::string _stringValue;
    XXvector _vectorValue;

public:
    virtual ~XXobject(){}
    void operator=(const XXobject &object){
        if (_type != object._type){
            return;
        }
        copy(object);
    }

    bool isInt()    { return XXobjectType_Int       == _type; }
    bool isDouble() { return XXobjectType_Double    == _type; }
    bool isString() { return XXobjectType_String    == _type; }
    bool isVector() { return XXobjectType_Vector    == _type; }
    
    int toInt()             { return _intValue; }
    double toDouble()       { return _doubleValue; }
    std::string toString()  { return _stringValue; }
    XXvector toVector()     { return _vectorValue; }

protected:
    virtual void copy(const XXobject &object){}
};

class XXintObject : public XXobject {
public:
    XXintObject(int value = 0);
    operator int() const;
protected:
    void copy(const XXobject &object);
};

class XXdoubleObject : public XXobject {
public:
    XXdoubleObject(double value = 0);
    operator double() const;
protected:
    void copy(const XXobject &object);
};

class XXstringObject : public XXobject {
public:
    XXstringObject(std::string value = "");
    operator std::string() const;
protected:
    void copy(const XXobject &object);
};

class XXvectorObject : public XXobject{
public:
    XXvectorObject(XXvector xxvector = XXvector());
    operator XXvector() const;
protected:
    void copy(const XXobject &object);
};
#endif
