#include "SqlDatabaseManager.h"
#include <QSqlDatabase>
#include <QFile>
#include <QDir>
#include <QList>

#include "XXlogger.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>

#include <QSqlDriver>

SqlDatabaseManager::SqlDatabaseManager(QObject *parent)
    : QObject(parent)
    , _isOpen(false){

}
SqlDatabaseManager::~SqlDatabaseManager(){
    if(_isOpen){
        this->close();
    }
}

bool SqlDatabaseManager::open(const QString &path){
    if(_isOpen){
        xxLog("can not open database again.");
        return false;
    }
    _isOpen = true;

    // 设置数据库的name和路径，当没有数据库文件时自动创建一个
	QFileInfo fileInfo(path);
	if (!fileInfo.dir().exists()) {
		QDir().mkpath(fileInfo.absolutePath());
	}

    QString fileName = fileInfo.fileName();
    _name = fileName.replace(".","_");
    if(QSqlDatabase::contains(_name)){
        _database = QSqlDatabase::database(_name);
    }
    else{
        _database = QSqlDatabase::addDatabase("QSQLITE", _name);
        _database.setDatabaseName(path);
    }


    // 打开数据库
    if(!_database.open()){
		xxLog("failure to open database. path:%s error:%s", path.toLocal8Bit().data(), _database.lastError().text().toLocal8Bit().data());
        _isOpen = false;
        return false;
    }

    //XXDebug("open database succeed. path:%s", path.toLocal8Bit().data());
    return true;
}
bool SqlDatabaseManager::createTable(const QMap<QString,QString> &keyAndType, const QString &tableName){
    if(!_isOpen || 0 == keyAndType.count() || tableName.isEmpty()){
		xxLog("database not open or invalid arg.");
        return false;
    }

    if(tableExisted(tableName)){
		xxLog("table is existed. tableName:%s", tableName.toLocal8Bit().data());
        return false;
    }

    QString ktString;
    QMap<QString,QString>::const_iterator iter = keyAndType.constBegin();
    for(; keyAndType.constEnd()!=iter; iter++){
        ktString += iter.key() + " " + iter.value();
        if(iter+1 != keyAndType.constEnd()){
            ktString += ",";
        }
    }

    QString sqlString = QString("create table %1 (%2);").arg(tableName).arg(ktString);
    QSqlQuery sqlQuery(_database);
    if(!sqlQuery.exec(sqlString)){
		xxLog("create table failure. tableName:%s sql:%s error:%s",
                tableName.toLocal8Bit().data(),
                sqlString.toLocal8Bit().data(),
                sqlQuery.lastError().text().toLocal8Bit().data());
        return false;
    }

    //XXDebug("create table succeed. tableName:%s", tableName.toLocal8Bit().data());
    return true;
}
bool SqlDatabaseManager::tableExisted(const QString &tableName){
    if(!_isOpen || tableName.isEmpty()){
		xxLog("database not open or invalid arg.");
        return false;
    }

    QString sqlString = QString("select * from %1;").arg(tableName);
    QSqlQuery sqlQuery(_database);
    if(!sqlQuery.exec(sqlString)){
        return false;
    }
    return true;
}

bool SqlDatabaseManager::keyExisted(const QString &key, const QString &tableName){
    if(!_isOpen || key.isEmpty() || tableName.isEmpty()){
		xxLog("database not open or invalid arg.");
        return false;
    }

    QString sqlString = QString("select * from %1;").arg(tableName);
    QSqlQuery sqlQuery(_database);
    if (!sqlQuery.exec(sqlString)){
		xxLogStr("table not exist. tableName:"+tableName);
        return false;
    }

    QSqlRecord record = sqlQuery.record();
    int index = record.indexOf(key);
    if(-1==index){
        return false;
    }

    return true;
}
bool SqlDatabaseManager::addKey2Table(const QString &key, const QString &type, const QString &tableName){
    if(!_isOpen || key.isEmpty() || type.isEmpty() || tableName.isEmpty()){
		xxLog("database not open or invalid arg.");
        return false;
    }

    if(keyExisted(key, tableName)){
		xxLog("key is existed. key:%s tableName:%s",
                key.toLocal8Bit().data(),
                tableName.toLocal8Bit().data());
        return false;
    }

    QString sqlString = QString("alter table %1 add %2 %3;").arg(tableName).arg(key).arg(type);
    QSqlQuery sqlQuery(_database);
    if(!sqlQuery.exec(sqlString)){
		xxLog("add key failure. key:%s type:%s tableName:%s sql:%s error:%s",
                key.toLocal8Bit().data(),
                type.toLocal8Bit().data(),
                tableName.toLocal8Bit().data(),
                sqlString.toLocal8Bit().data(),
                sqlQuery.lastError().text().toLocal8Bit().data());
        return false;
    }

    //XXDebug("add key succeed. key:%s type:%s tableName:%s",
    //        key.toLocal8Bit().data(),
    //        type.toLocal8Bit().data(),
    //        tableName.toLocal8Bit().data());
    return true;
}
bool SqlDatabaseManager::removeKeyAtTable(const QString &key, const QString &tableName){
    return false;
    if(!_isOpen || key.isEmpty() || tableName.isEmpty()){
		xxLog("database not open or invalid arg.");
        return false;
    }

    QString sqlString = QString("alter table %1 DROP column %2;").arg(tableName).arg(key);
    QSqlQuery sqlQuery(_database);
    if(!sqlQuery.exec(sqlString)){
		xxLog("remove key failure. key:%s tableName:%s sql:%s error:%s",
                key.toLocal8Bit().data(),
                tableName.toLocal8Bit().data(),
                sqlString.toLocal8Bit().data(),
                sqlQuery.lastError().text().toLocal8Bit().data());
        return false;
    }

    //XXDebug("remove key succeed. key:%s type:%s tableName:%s",
    //        key.toLocal8Bit().data(),
    //        tableName.toLocal8Bit().data());
    return 0;
}
bool SqlDatabaseManager::removeMapAtTable(const QMap<QString, QVariant> &equalTo, const QString &tableName) {
	QString eqString;
	for (auto iter = equalTo.constBegin(); iter != equalTo.constEnd(); iter++) {
		eqString += iter.key();
		eqString += "=?";
		if (iter + 1 != equalTo.constEnd()) {
			eqString += " AND ";
		}
	}

	QString sqlString = QString("DELETE FROM %1 WHERE %2;").arg(tableName).arg(eqString);
	QSqlQuery sqlQuery(_database);
	sqlQuery.prepare(sqlString);
	for (auto iter = equalTo.constBegin(); iter != equalTo.constEnd(); iter++) {
		sqlQuery.addBindValue(iter.value());
	}

	if (!sqlQuery.exec()) {
		xxLog("delete map failure. tableName:%s sql:%s error:%s",
			tableName.toLocal8Bit().data(),
			sqlString.toLocal8Bit().data(),
			sqlQuery.lastError().text().data());
		return false;
	}
	else if (sqlQuery.numRowsAffected() <= 0) {
		xxLog("can not find anything, failure to delete values. tableName:%s", tableName.toLocal8Bit().data());
	}
	else {
		//xxLog("delete values succeed. tableName:%s", tableName.toLocal8Bit().data());
	}
	return true;
}
bool SqlDatabaseManager::setValues2Table(const QMap<QString,QVariant> &values, const QMap<QString,QVariant> &equalTo, const QString &tableName){
    if(!_isOpen || values.isEmpty() || equalTo.isEmpty() || tableName.isEmpty()){
		xxLog("database not open or invalid arg.");
        return false;
    }

    QString vString;
    for (auto iter=values.constBegin(); iter!=values.constEnd(); iter++) {
        vString += iter.key();
        vString += "=?";
        if(iter+1!=values.constEnd()){
            vString +=",";
        }
    }

    QString eqString;
    for (auto iter=equalTo.constBegin(); iter!=equalTo.constEnd(); iter++) {
        eqString += iter.key();
        eqString += "=?";
        if(iter+1!=equalTo.constEnd()){
            eqString += " AND ";
        }
    }

    QString sqlString = QString("UPDATE %1 SET %2 WHERE %3;").arg(tableName).arg(vString).arg(eqString);
    QSqlQuery sqlQuery(_database);
    sqlQuery.prepare(sqlString);
    for (auto iter=values.constBegin(); iter!=values.constEnd(); iter++) {
        sqlQuery.addBindValue(iter.value());
    }
    for (auto iter=equalTo.constBegin(); iter!=equalTo.constEnd(); iter++) {
        sqlQuery.addBindValue(iter.value());
    }

    if(!sqlQuery.exec()){
		xxLog("set values failure. tableName:%s sql:%s error:%s",
                tableName.toLocal8Bit().data(),
                sqlString.toLocal8Bit().data(),
                sqlQuery.lastError().text().data());
        return false;
    }
    else if(sqlQuery.numRowsAffected()<=0){
		xxLog("can not find anything, failure to set values. tableName:%s", tableName.toLocal8Bit().data());
    }
    else{
		//xxLog("set values succeed. tableName:%s", tableName.toLocal8Bit().data());
    }
    return true;
}
bool SqlDatabaseManager::addMap2Table(const QMap<QString,QVariant> &values, const QString &tableName){
    if(!_isOpen || values.isEmpty() || tableName.isEmpty()){
		xxLog("database not open or invalid arg.");
        return false;
    }

    QString kString, vString;
    for (auto iter=values.constBegin(); iter!=values.constEnd(); iter++) {
        kString += iter.key();
        vString += "?";
        if(iter+1!=values.constEnd()){
            kString += ",";
            vString += ",";
        }
    }

    QString sqlString = QString("insert into %1 (%2) values (%3);").arg(tableName).arg(kString).arg(vString);
    QSqlQuery sqlQuery(_database);
    sqlQuery.prepare(sqlString);
    for (auto iter=values.constBegin(); iter!=values.constEnd(); iter++) {
        sqlQuery.addBindValue(iter.value());
    }

    if(!sqlQuery.exec()){
		xxLog("add map failure. tableName:%s sql:%s error:%s",
                tableName.toLocal8Bit().data(),
                sqlString.toLocal8Bit().data(),
                sqlQuery.lastError().text().data());
        return false;
    }

    //XXDebug("add map succeed. tableName:%s", tableName.toLocal8Bit().data());
    return true;
}
bool SqlDatabaseManager::addMaps2Table(const QList<QVariantMap> &maps, const QString &tableName){
    _database.driver()->beginTransaction();
    bool ret = false;
    foreach(auto map,maps){
        ret = addMap2Table(map,tableName);
        if(!ret){
            break;
        }
    }
    if(ret){
        _database.driver()->commitTransaction();
    }
    else{
        _database.driver()->rollbackTransaction();
    }
    return ret;
    /*
    if(!_isOpen || maps.isEmpty() || tableName.isEmpty()){
        xxLog("database not open or invalid arg.");
        return false;
    }    
    QSqlQuery sqlQuery(_database);
    for (int index=0; index<maps.count(); index++) {
        QVariantMap values = maps[index];
        QString kString, vString;
        for (auto iter=values.constBegin(); iter!=values.constEnd(); iter++) {
            kString += iter.key();
            vString += "?";
            if(iter+1!=values.constEnd()){
                kString += ",";
                vString += ",";
            }
        }

        QString sqlString = QString("insert into %1 (%2) values (%3);").arg(tableName).arg(kString).arg(vString);
        sqlQuery.prepare(sqlString);
        for (auto iter=values.constBegin(); iter!=values.constEnd(); iter++) {
            sqlQuery.addBindValue(iter.value());
        }
    }

    if(!sqlQuery.exec()){
        xxLog("add maps failure. tableName:%s error:%s",
                tableName.toLocal8Bit().data(),
                sqlQuery.lastError().text().data());
        return false;
    }
    return true;
    */
}
QList<QVariantMap> SqlDatabaseManager::getMapFromTable(const QString &tableName, const QVariantMap &equelTo){
    QList<QVariantMap> list;
    if(!_isOpen || tableName.isEmpty()){
		xxLog("database not open or invalid arg.");
        return list;
    }

    QString eqString;
    for (auto iter=equelTo.constBegin(); iter!=equelTo.constEnd(); iter++) {
        eqString += iter.key();
        eqString += "=?";
        if(iter+1!=equelTo.constEnd()){
            eqString += " AND ";
        }
    }

    QString sqlString = eqString.isEmpty() ? QString("SELECT * FROM %1;").arg(tableName) : QString("SELECT * FROM %1 WHERE %2;").arg(tableName).arg(eqString);
    QSqlQuery sqlQuery(_database);
    sqlQuery.prepare(sqlString);

    for (auto iter=equelTo.constBegin(); iter!=equelTo.constEnd(); iter++) {
        sqlQuery.addBindValue(iter.value());
    }

    if(!sqlQuery.exec()){
		xxLog("failure to get map. tableName:%s sql:%s error:%s",
                tableName.toLocal8Bit().data(),
                sqlString.toLocal8Bit().data(),
                sqlQuery.lastError().text().data());
    }
    else{
        while(sqlQuery.next()){
           QSqlRecord record = sqlQuery.record();
           int count = record.count();
           QVariantMap kvMap;
           for (int index=0; index<count; index++) {
               kvMap[record.fieldName(index)] = record.value(index);
           }
           list << kvMap;
        }
    }

    return list;
}

int SqlDatabaseManager::getKeyMaxFromTable(const QString &tableName, const QString &key) {
	if (!_isOpen || tableName.isEmpty()) {
		xxLog("database not open or invalid arg.");
		return -1;
	}

	QString sqlString = QString("SELECT MAX(%1) FROM %2;").arg(key).arg(tableName);
	QSqlQuery sqlQuery(_database);
	if (!sqlQuery.exec(sqlString)) {
		xxLog("failure to get key max. tableName:%s sql:%s error:%s",
			tableName.toLocal8Bit().data(),
			sqlString.toLocal8Bit().data(),
			sqlQuery.lastError().text().data());
		return -1;
	}

	sqlQuery.next();
	return sqlQuery.value(0).toInt();
}

bool SqlDatabaseManager::close(){
    if(!_isOpen){
		xxLog("none database is opening.");
        return false;
    }

    _isOpen = false;

    QString connection = _database.connectionName();
    _database.close();
    _database = QSqlDatabase();
    _database.removeDatabase(connection);
    return true;
}
