#ifndef NETWORKDATA_H
#define NETWORKDATA_H

#include <QObject>
#include<QByteArray>
#include<QList>
#include<QString>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>
#include<QJsonDocument>
#include<QDebug>

struct FileInfo{
    QString user;
    QString md5;
    QString createTime;
    QString fileName;
    int shareStatus;
    int pv;
    QString url;
    int size;
    QString type;
};

class NetworkData : public QObject
{
    Q_OBJECT
public:
    explicit NetworkData(QObject *parent = nullptr);


    static QList<FileInfo*> getFileInfo(QByteArray json);
    static QStringList getFileCount(QByteArray json);
    static QString getCode(QByteArray json);

signals:

};

#endif // NETWORKDATA_H
