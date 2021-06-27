#ifndef SQLDATABASEMANAGER_H
#define SQLDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QMap>

class SqlDatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit SqlDatabaseManager(QObject *parent = nullptr);
    virtual ~SqlDatabaseManager();

    /**
     * @brief 打开数据库，当前路径没有数据库文件时，创建一个新的数据库
     * @param path 数据路径，以xx.db结尾
     * @return true:成功
     */
    bool open(const QString &path);

    /**
     * @brief 创建数据库表
     * @param keyAndType 数据库表字段和类型
     * @param tableName 数据库表名词
     * @return true:成功
     */
    bool createTable(const QMap<QString,QString> &keyAndType, const QString &tableName);

    /**
     * @brief 查询数据库表是否存在
     */
    bool tableExisted(const QString &tableName);

    /**
     * @brief 查询数据库表中是否存在某字段
     */
    bool keyExisted(const QString &key, const QString &tableName);

    /**
     * @brief 插入一个字段到数据库表中
     * @param key 需要插入字段
     * @param type 字段对应的数据类型，考虑使用枚举，如：int、float、bool...
     * @param tableName 插入的目标数据库表名称
     * @return true:成功
     */
    bool addKey2Table(const QString &key, const QString &type, const QString &tableName);

    /**
     * @brief 从数据库表中删除字段（注意：sqlite不支持删除key，所以此函数直接返回false）
     * @param key 需要删除的字段
     * @param tableName 需要删除字段的数据库表名
     * @return true:成功
     */
    bool removeKeyAtTable(const QString &key, const QString &tableName);

	/**
	 * 移除表数据
	 */
	bool removeMapAtTable(const QMap<QString, QVariant> &equalTo, const QString &tableName);

    /**
     * @brief 找出能够匹配的数据表，并对其进行值修改
     * @param values 需要修改的键值对
     * @param equalTo 需要匹配的值
     * @param tableName 数据库表名称
     * @return true:成功
     */
    bool setValues2Table(const QMap<QString,QVariant> &values, const QMap<QString,QVariant> &equalTo,const QString &tableName);

    /**
     * @brief 插入一个数据表
     * @param values 表中的所有内容
     * @param tableName 数据库
     * @return
     */
    bool addMap2Table(const QMap<QString,QVariant> &values, const QString &tableName);
    bool addMaps2Table(const QList<QVariantMap> &maps, const QString &tableName);

    QList<QVariantMap> getMapFromTable(const QString &tableName, const QVariantMap &equelTo = QVariantMap());

	int getKeyMaxFromTable(const QString &tableName, const QString &key);

    /**
     * @brief 关闭数据库
     */
    bool close();

    /**
     * @brief 已经打开一个数据库
     */
    bool isOpen() const { return _isOpen; }

signals:

public slots:

private:
    bool _isOpen;
    QString _name;
    QSqlDatabase _database;
};

#endif // SQLDATABASEMANAGER_H
