#ifndef XXpi_h
#define XXpi_h

#include <string>
#include <map>
#include <memory>

using namespace std;
class XXpi{
protected:
    XXpi(const string &subPi);
public:
    virtual ~XXpi();
    
public:
    shared_ptr<string> getData();
    bool parseData(const int8_t *data, int length, map<string,string> &map);

protected:
    void resetPairs();
    XXpi& addPairToLocalMap(const string &key, const string &value);
    XXpi& removePairAtLocalMap(const string &key);

private:
    void addPairToString(shared_ptr<string> &stringPtr, const string &key, const string &value);

private:
    string _subPi;
    map<string,string> _map;
};

#endif
