#ifndef XXobject_h
#define XXobject_h

#include <stdio.h>
#include <string>

class XXobject {
protected:
    XXobject();
public:
    virtual ~XXobject();
    
    virtual bool isInt()    { return false; }
    virtual bool isDouble() { return false; }
    virtual bool isString() { return false; }
    //virtual bool isObject() { return false; }
    
    virtual int toInt()             { return 0; }
    virtual double toDouble()       { return 0; }
    virtual std::string toString()  { return ""; }
//    template <class T>
//    T* toObject()           { return this; }
};

class XXintObject : public XXobject {
public:
    XXintObject(int value = 0) : XXobject(), _value(value){}
    bool isInt()    { return true; }
    int toInt()     { return _value; }
private:
    int _value;
};

class XXdoubleObject : public XXobject {
public:
    XXdoubleObject(double value = 0) : XXobject(), _value(value){}
    bool isDouble()     { return true; }
    double toDouble()   { return _value; }
private:
    double _value;
};

class XXstringObject : public XXobject {
public:
    XXstringObject(std::string value = "") : XXobject(), _value(value){}
    bool isString()         { return true; }
    std::string toString()  { return _value; }
private:
    std::string _value;
};
#endif
