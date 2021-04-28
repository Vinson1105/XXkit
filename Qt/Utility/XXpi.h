#ifndef XXPI_H
#define XXPI_H

#include <QByteArray>
#include <QJsonObject>

/**
 * @brief The XXpi class
 * 基于TCP的数据结构协议如下（TCP有沾包的情况，所以需要）
 * [XX]\r\n
 * type\r\n
 * data\r\n\r\n
 *
 * 其中
 * :XX是固定标识
 * type是data数据类型，
 * data是实际数据
 */
namespace XXpi{
    static QByteArray combine(const QString &type, const QByteArray &payload);
    static qint64 parse(const QByteArray &bytes, QString &oType, QByteArray &oPayload);
};

#endif // XXPI_H
