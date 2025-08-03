#include "common.h"
#include<QCryptographicHash>
#include"des.h"
#include<QDebug>
#include<QJsonDocument>
#include<QFile>
#include<QFileInfo>
#include<QJsonObject>
#include<QObject>
#include<QDir>
#include<QSaveFile>
QMutex Common::logMutex;
Common * Common:: m_instance=new Common;

Common::Common()
{
manager=new QNetworkAccessManager(this);
getFileTypeList();


}

Common *Common::getInstance(){
    if (m_instance == nullptr) {
            m_instance = new Common();
        }
    return m_instance;
}

QString Common::getConfValue(QString title,QString key,QString path){
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"open json fail";
        return "";
    }

    QByteArray json= file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc =QJsonDocument::fromJson(json,&err);
    if(err.error==QJsonParseError::NoError){
        if(doc.isNull()||doc.isEmpty()){
            qDebug()<<"doc is null or empty";
            return "";
        }

    if(doc.isObject()){
        QJsonObject rootObj=doc.object();
        QJsonValue titleValue = rootObj.value(title);
        if(titleValue.type()==QJsonValue::Object){
            QJsonObject titleObj=titleValue.toObject();

            QStringList list =titleObj.keys();
            for(int i=0;i<list.size();i++){
                QString keyTemp =list.at(i);
                if(keyTemp==key){
                    return titleObj.value(key).toString();
                }
            }
        }
    }




    }
    else{
        qDebug()<<"err ="<<err.errorString();
    }
    return "";
}

void Common::writeLoginInfo(QString user,QString pwd,bool isRemember,QString path){

//1 des加密 (较为过时

    unsigned char encUsr[1024]={0};
    int encUsrLen;
    int ret=DesEnc((unsigned char*)user.toLocal8Bit().data(),user.toLocal8Bit().length()
                   ,encUsr,&encUsrLen);
    if(ret!=0){
        qDebug()<<"DesEnc 加密用户名失败";
        return;
    }

    unsigned char encPwd[1024]={0};
    int encPwdLen;
    ret=DesEnc((unsigned char*)pwd.toLocal8Bit().data(),pwd.toLocal8Bit().length()
                   ,encPwd,&encPwdLen);
    if(ret!=0){
        qDebug()<<"DesEnc 加密密码失败";
        return;
    }
//2 base64加密  解决二进制数据的存储和传输问题 将二进制转换为 ASCII 字符串
    QString base64User =QByteArray((char *)encUsr,encUsrLen).toBase64();
    QString base64Pwd =QByteArray((char *)encPwd,encPwdLen).toBase64();

    QMap<QString,QVariant> login;
    login.insert("pwd",base64Pwd);
    login.insert("user",base64User);
    if(isRemember==1){
        login.insert("remember","yes");
    }else{
        login.insert("remember","no");
    }


    QString ip= getConfValue("web_server","ip");
    QString port= getConfValue("web_server","port");

    QMap<QString,QVariant> web_server;
    web_server.insert("ip",ip);
    web_server.insert("port",port);

    QMap<QString,QVariant> json;
    json.insert("web_server",web_server);
    json.insert("login",login);

    QJsonDocument jsonDocument=QJsonDocument::fromVariant(json);
    if(jsonDocument.isNull()){
        qDebug()<<"QJsonDocument::fromVariant fail";
        return;
    }

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<"writeonly open err";
        return;
    }
    file.write(jsonDocument.toJson());
    file.close();


};

void Common::writeWebInfo(QString ip, QString port, QString path){
    QMap<QString,QVariant> web_server;
    web_server.insert("ip",ip);
    web_server.insert("port",port);

    QString user=getConfValue("login","user");
    QString pwd=getConfValue("login","pwd");
    QString remember=getConfValue("login","remember");

    QMap<QString,QVariant> login;
    login.insert("user",user);
    login.insert("pwd",pwd);
    login.insert("remember",remember);

    QMap<QString,QVariant> json;
    json.insert("web_server",web_server);
    json.insert("login",login);

    QJsonDocument jsonDocument=QJsonDocument::fromVariant(json);
    if(jsonDocument.isNull()){
        qDebug()<<"QJsonDocument::fromVariant fail";
        return;
    }

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)){
        qDebug()<<"writeonly open err";
        return;
    }
    file.write(jsonDocument.toJson());
    file.close();

}

QString Common::getStrMd5(QString str)
{
    QByteArray arr;
    arr=QCryptographicHash::hash(str.toLocal8Bit(),QCryptographicHash::Md5);
    return arr.toHex();
}

QString Common::getFileMd5(QString filePath){
    QFile localFile(filePath);

    if(!localFile.open(QFile::ReadOnly)){
        qDebug()<<"file open error";
        return QString();
    }

    QCryptographicHash ch(QCryptographicHash::Md5);
    const quint64 loadSize = 1024 * 4;
    QByteArray buf;

    while(!localFile.atEnd()){
        buf = localFile.read(loadSize);
        if(buf.isEmpty()) break;
        ch.addData(buf);
    }
    localFile.close();
    return ch.result().toHex();
}



QNetworkAccessManager* Common::getNetworkAccessManager(){
    return manager;
}

void Common::getFileTypeList(){
    QDir dir(FILE_TYPE_DIR);
    if(!dir.exists()){
        dir.mkpath(FILE_TYPE_DIR);
        qDebug()<<FILE_TYPE_DIR<<"successfully build";
    }

    dir.setFilter(QDir::Files|QDir::NoDot|QDir::NoDotDot|QDir::NoDotAndDotDot|QDir::NoSymLinks);
    dir.setSorting(QDir::Size);

    QFileInfoList fileInfoList=dir.entryInfoList();
    for(int i=0;i<fileInfoList.size();i++){
        QFileInfo fileInfo=fileInfoList.at(i);
        m_FileTypeList.append(fileInfo.fileName());
    }

}

void Common::writeRecord(QString user,QString fileName,QString code,QString path){
    QString recordDir = path;
      QString recordFilePath = recordDir + "/" + user + ".txt";

      // 1) 确保目录存在
      QDir dir;
      if (!dir.exists(recordDir)) {
          if (!dir.mkpath(recordDir)) {
              qDebug() << "目录创建失败:" << recordDir;
              return;
          }
      }

      // 2) 读取原有内容（如果你确实想把新日志写在文件最前面）
      QByteArray old;
      QFile file(recordFilePath);
      if (file.exists()) {
          if (!file.open(QIODevice::ReadOnly)) {
              qDebug() << "无法以只读模式打开日志文件:" << recordFilePath;
              return;
          }
          old = file.readAll();
          file.close();
      }

      // 3) 准备写入新日志（追加模式或前插都行）
      if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
          qDebug() << "无法以写入模式打开日志文件:" << recordFilePath;
          return;
      }

      // 时间戳 + 操作
      QString timeStr = QDateTime::currentDateTime()
                            .toString("yyyy/MM/dd hh:mm:ss");
      QString action = getActionString(code);
      QString line = QString("%1\t%2\t%3\n")
                         .arg(fileName)
                         .arg(timeStr)
                         .arg(action);

      // 4) 写新日志＋旧内容
      file.write(line.toLocal8Bit());
      file.write(old);
      file.close();
  }


QString Common::getActionString(QString code){
    QString str;
    if(code=="005"){
        str="文件已存在";
    }else if(code=="006"){
        str="秒传成功";
    }else if(code=="007"){
        str="秒传失败";
    }else if(code=="008"){
        str="上传成功";
    }else if(code=="009"){
        str="上传失败";
    }else if(code=="090"){
        str="下载成功";
    }else if(code=="091"){
        str="下载失败";
    }
    return str;

}


