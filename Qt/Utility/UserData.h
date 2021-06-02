#ifndef USERDATA_H
#define USERDATA_H

#include <QObject>
#include <QScopedPointer>
#include "XXjson.h"

class UserData : public QObject
{
    Q_OBJECT
public:    
    static void reset(const QString &config, const QString &setting);
    static XXjson& config();
    static XXjson& setting();

private:
    static UserData *instance();
    explicit UserData(QObject *parent = nullptr);
    void reset_(const QString &config, const QString &setting);
    XXjson& config_();
    XXjson& setting_();

private:
    static UserData *_instance;
    QScopedPointer<XXjson> _config;
    QScopedPointer<XXjson> _setting;
};

#endif // USERDATA_H
