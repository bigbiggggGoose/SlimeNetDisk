#ifndef COMMON_H
#define COMMON_H

#include<QString>
#include"des.h"
#include<QNetworkAccessManager>
#include<QStringList>
#include <QMutex>

//正则表达式

//  \\d 允许数字   {1,16}允许1到16个单位
// (?=.*[a-z])(?=.*[A-Z])(?=.*\\d) 至少需要一个小写字母、大写字母、数字
// "1\\d{10}" 1开头 后面只允许10个数字 共11个  "1[3-9]\\d{9}"也行
// 邮箱严格些可"^[a-zA-Z\\d\\._-]+@[a-zA-Z\\d-]+(\\.[a-zA-Z\\d-]+)+\\.[a-zA-Z]{2,}$"
// 端口分段匹配 1-9 10-99 100-999 1000-9999 10000-65535   $)|
#define USER_REG "^[a-zA-Z\\d_@#-\\*]{1,16}$"
#define PASSWD_REG "^[a-zA-Z\\d_@#-\\*]{6,18}$"
#define PHONE_REG "1\\d{10}"
#define EMAIL_REG "^[a-zA-Z\\d\._-]\+@[a-zA-Z\\d_\.-]\+(\.[a-zA-Z0-9_-]\+)+$"
#define IP_REG "((25[0-5]|2[0-4]\\d|[01]?\\d\\d?)\\.){3}(25[0-5]|2[0-4]\\d|[01]?\\d\\d?)"
#define PORT_REG "^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)"

#define CFG_FILE "D:/QT/Project/SlimeNetdisk/conf/cfg.json"
#define FILE_TYPE_DIR "Resource/TypeIcon"
#define RECORD_DIR  "D:/QT/Project/SlimeNetdisk/conf/record"

enum TransmitStatus{
    Work,
    Record
};

class Common: public QObject
{
private:
    Common();//唯一
     static Common * m_instance;
     QNetworkAccessManager *manager;
     QStringList m_FileTypeList;
     static QMutex logMutex;
public:



    static Common *getInstance();
    QNetworkAccessManager* getNetworkAccessManager();

    //读取文件数据
    QString getConfValue(QString title,QString key,QString path=CFG_FILE);


    void writeLoginInfo(QString username,QString password,bool isRemember,QString path=CFG_FILE);

    void writeWebInfo(QString ip,QString port,QString path=CFG_FILE);


    QString getStrMd5(QString str);//md5加密字符串

    QString getFileMd5(QString filePath);

    QString getBoundary();

    void getFileTypeList();

    void writeRecord(QString user,QString fileName,QString code,QString path=RECORD_DIR);
    QString getActionString(QString code);

};

#endif // COMMON_H
