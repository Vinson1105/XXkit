#ifndef XXpi_h
#define XXpi_h

#include <string>
#include <map>

using namespace std;
class XXpi{
protected:
    XXpi(const string &subPi);
public:
    virtual ~XXpi();
    
public:
    const string& getData();
    bool parseData(const int8_t *data, int length, map<string,string> &map);

protected:
    void resetPairs();
    XXpi& addPair(const string &key, const string &value);

private:
    string _subPi;
    map<string,string> _map;
};

#endif
