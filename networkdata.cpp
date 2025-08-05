#include "networkdata.h"

NetworkData::NetworkData(QObject *parent) : QObject(parent)
{

}

QList<FileInfo*> NetworkData::getFileInfo(QByteArray json){
    QList<FileInfo*> list;
 qDebug().noquote() << "原始JSON数据:" << json;
    QJsonParseError err;
    QJsonDocument rootDoc=QJsonDocument::fromJson(json,&err);
    if(err.error!=QJsonParseError::NoError){
        qDebug()<<"json格式错误";
        return list;
    }
        //解析
        QJsonObject rootObj=rootDoc.object();


/*

*/

        QJsonValue filesValue=rootObj.value("files");
        if(filesValue.type()==QJsonValue::Array){
            QJsonArray filesArr=filesValue.toArray();

            for(int i=0;i<filesArr.count();i++){
                QJsonValue fileValue=filesArr.at(i);
                if(fileValue.type()==QJsonValue::Object){

                    FileInfo *fileInfo=new FileInfo;

                    QJsonObject fileObj=fileValue.toObject();
                    QJsonValue userValue=fileObj.value("user");
                    QJsonValue md5Value=fileObj.value("md5");
                    QJsonValue createtimeValue=fileObj.value("create_time");
                    QJsonValue fileNameValue=fileObj.value("file_name");
                    QJsonValue shareStatus=fileObj.value("share_status");
                    QJsonValue pvValue=fileObj.value("pv");
                    QJsonValue urlValue=fileObj.value("url");
                    QJsonValue sizeValue=fileObj.value("size");
                    QJsonValue typeValue=fileObj.value("type");


                    fileInfo->user=userValue.toString();
                    fileInfo->md5=md5Value.toString();
                    fileInfo->createTime=createtimeValue.toString();
                    fileInfo->fileName=fileNameValue.toString();
                    fileInfo->shareStatus=shareStatus.toInt();
                    fileInfo->pv=pvValue.toInt();
                    fileInfo->url=urlValue.toString();
                    fileInfo->size=sizeValue.toInt();
                    fileInfo->type=typeValue.toString();

                    qDebug() << "解析文件:" << fileInfo->fileName
                             << "上传者:" << fileInfo->user;

                    list.append(fileInfo);
                }
            }
        }



 return list;
}

QString NetworkData::getCode(QByteArray json){
    QString code;

    QJsonParseError err;
    //解析数据
    QJsonDocument rootDoc=QJsonDocument::fromJson(json,&err);
    if(err.error !=QJsonParseError::NoError){
        qDebug()<<"wrong json1";
    }else {
//
QJsonObject rootObj=rootDoc.object();
QJsonValue codeValue=rootObj.value("code");
code=codeValue.toString();


   }

   return code;
}


QStringList NetworkData::getFileCount(QByteArray json){
    QStringList list;

    QJsonParseError err;
    //解析数据
    QJsonDocument rootDoc=QJsonDocument::fromJson(json,&err);
    if(err.error !=QJsonParseError::NoError){
        qDebug()<<"wrong json2";
    }else {
//
QJsonObject rootObj=rootDoc.object();
QJsonValue codeValue=rootObj.value("code");

/*
{
"num"="1",
"code"="110"
}
*/

if(codeValue.type()==QJsonValue::String){
    QString code=codeValue.toString();
    list.append(code);

 }

QJsonValue numValue=rootObj.value("num");
if(codeValue.type()==QJsonValue::String){
    QString num=numValue.toString();
    list.append(num);

 }


   }

   return list;
}
