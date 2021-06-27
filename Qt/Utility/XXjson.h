#ifndef XXapplicationSetting_h
#define XXapplicationSetting_h

#include <QObject>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include <QJsonArray>

class XXjsonItem
{
    friend class XXjson;
public:
    XXjsonItem(const QVariant &key="", const QJsonValue &jvalue=QJsonValue(), XXjsonItem *parent=nullptr);
    ~XXjsonItem();

    XXjsonItem operator[](QVariant key);
    void operator=(QVariant value);
    void operator=(const XXjsonItem &item);

    QString toString() const;
    int toInt() const ;
    bool containts(QVariant key);

    void saveAtParent();

private:
    QVariant _key;
    QJsonValue _jvalue;
    XXjsonItem *_parent;
};

class XXjson
{
public:
    XXjson();
    XXjson(const QJsonObject &jobj);
    XXjson(const QString &path);
    virtual ~XXjson();

    XXjsonItem operator[](const QString &key);
    QJsonObject operator()(void);

    bool loadFromFile(const QString &path);
    bool saveToFile(const QString &path = "");

private:
    bool loadFromFile(const QString &path, QJsonObject &jobj);

private:
    QString _path;
    XXjsonItem _root;
};

#endif
