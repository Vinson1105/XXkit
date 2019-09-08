#ifndef XXobject_h
#define XXobject_h

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

typedef enum XXobjectType{
    XXobjectType_Int            = 1,        
    XXobjectType_Double,
    XXobjectType_String,
    XXobjectType_Vector,
    XXobjectType_Map,
    XXobjectType_CustomBegin    = 100,
} XXobjectType;
class XXobject;
typedef std::shared_ptr< XXobject > XXobjectShared;
typedef std::vector< XXobjectShared > XXobjectSharedVector;
typedef std::map< std::string, XXobjectShared > XXobjectSharedMap;

class XXintObject;
class XXdoubleObject;
class XXstringObject;
class XXvectorObject;
class XXmapObject;
class XXobject {
    friend class XXintObject;
    friend class XXdoubleObject;
    friend class XXstringObject;
    friend class XXvectorObject;
    friend class XXmapObject;
protected:
    XXobject(XXobjectType type) 
        : _type(type)
        , _intValue(0)
        , _doubleValue(0)
        , _stringValue("")
        , _vectorValue(XXobjectSharedVector()){}
    XXobjectType _type; 

private: 
    int _intValue;                  
    double _doubleValue;
    std::string _stringValue;
    XXobjectSharedVector _vectorValue;
    XXobjectSharedMap _mapValue;

public:
    virtual XXobject* create() const { return nullptr; }
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
    bool isMap()    { return XXobjectType_Map       == _type; }
    
    int toInt()             { return _intValue; }
    double toDouble()       { return _doubleValue; }
    std::string toString()  { return _stringValue; }
    XXobjectSharedVector toVector() { return _vectorValue; }
    XXobjectSharedMap toMap()       { return _mapValue; }

protected:
    virtual void copy(const XXobject &object){}
};

class XXintObject : public XXobject {
public:
    XXintObject(int value = 0);
    XXobject* create() const;
    operator int() const;
protected:
    void copy(const XXobject &object);
};

class XXdoubleObject : public XXobject {
public:
    XXdoubleObject(double value = 0);
    XXobject* create() const;
    operator double() const;
protected:
    void copy(const XXobject &object);
};

class XXstringObject : public XXobject {
public:
    XXstringObject(std::string value = "");
    XXobject* create() const;
    operator std::string() const;
protected:
    void copy(const XXobject &object);
};

class XXvectorObject : public XXobject{
public:
    XXvectorObject(XXobjectSharedVector objectSharedVector = XXobjectSharedVector());
    XXobject* create() const;
    operator XXobjectSharedVector() const;
protected:
    void copy(const XXobject &object);
};

class XXmapObject : public XXobject{
public:
    XXmapObject(XXobjectSharedMap objectSharedMap = XXobjectSharedMap());
    XXobject* create() const;
    operator XXobjectSharedMap() const;
protected:
    void copy(const XXobject &object);
};
#endif
