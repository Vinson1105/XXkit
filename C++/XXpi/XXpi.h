#ifndef XXpi_h
#define XXpi_h

#include <string>
#include <map>
#include <memory>

class XXpi{
protected:
    XXpi(const std::string &subPi);
public:
    virtual ~XXpi();
    
public:
    std::shared_ptr<std::string> getData();
    bool parseData(const int8_t *data, int length, std::map<std::string,std::string> &map);
    int composeData(int8_t *data);
    int composeData(int8_t *data, const std::map<std::string,std::string> &map);

protected:
    void resetPairs();
    XXpi& addPairToLocalMap(const std::string &key, const std::string &value);
    XXpi& removePairAtLocalMap(const std::string &key);

private:
    void addPairToString(std::shared_ptr<std::string> &stringPtr, const std::string &key, const std::string &value);

private:
    std::string _subPi;
    std::map<std::string,std::string> _map;
};

#endif
