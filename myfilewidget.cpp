#include "myfilewidget.h"
#include "ui_myfilewidget.h"
#include<QListView>
#include<QDebug>
#include"mymenu.h"
#include<QAction>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include"common.h"
#include<QJsonObject>
#include<QJsonDocument>
#include<QMessageBox>
#include<QJsonValue>
#include<QJsonArray>
#include"networkdata.h"
#include<QFileInfo>
#include"filepropertyinfodialog.h"
#include<QFileDialog>
#include<QHttpMultiPart>
#include <QHttpPart>
#include<QDesktopServices>
MyFileWidget::MyFileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyFileWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|windowFlags());
    m_common=Common:: getInstance();
    m_loginInfo=LoginInfoInstance::getInstance();
    manager=m_common->getNetworkAccessManager();

    initListWidget();

    addMenu();
    menuActions();
    addUploadItem();

    m_uploadTask=new UploadTask;
    m_downloadTask=new DownloadTask;

    checkTaskList();
    connect(&m_timerForDownload,&QTimer::timeout,this,[=](){
    downloadFileAction();
    });
   }

MyFileWidget::~MyFileWidget()
{
    delete ui;
}

void MyFileWidget::initListWidget(){
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(80,80));
    ui->listWidget->setGridSize(QSize(100,200));


    ui->listWidget->setResizeMode(QListView::Adjust);


    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    //连接信号到onRightMenu函数,右键点击时触发
    connect(ui->listWidget,&QListWidget::customContextMenuRequested,this,&MyFileWidget::onRightMenu);
}

void MyFileWidget::onRightMenu(const QPoint &pos){
    QListWidgetItem *item=ui->listWidget->itemAt(pos);
    if(item){

        if(item->text()== "上传文件"){
        return;
        }
        m_inmenu->exec(QCursor::pos());
    }else{

        m_exmenu->exec(QCursor::pos());
    }
}

void MyFileWidget::addMenu(){
    m_inmenu=new MyMenu(this);

    actionDownload=new QAction("下载",this);
    actionShare=new QAction("分享",this);
    actionDelete=new QAction("删除",this);
    actionProperty=new QAction("属性",this);

    m_inmenu->addAction(actionDownload);
    m_inmenu->addAction(actionShare);
    m_inmenu->addAction(actionDelete);
    m_inmenu->addAction(actionProperty);

    //外菜单：
    m_exmenu=new MyMenu(this);

    actionAsc=new QAction("升序",this);
    actionDesc=new QAction("降序",this);
    actionRefresh=new QAction("刷新",this);
    actionUpload=new QAction("上传",this);

    m_exmenu->addAction(actionAsc);
    m_exmenu->addAction(actionDesc);
    m_exmenu->addAction(actionRefresh);
    m_exmenu->addAction(actionUpload);
}

void MyFileWidget::menuActions(){
    connect(actionDownload,&QAction::triggered,this,[=](){
        addDownloadFiles();
    });
    connect(actionShare,&QAction::triggered,this,[=](){
        dealFile("share");
    });
    connect(actionDelete,&QAction::triggered,this,[=](){
        dealFile("delete");
    });
    connect(actionProperty,&QAction::triggered,this,[=](){
        dealFile("property");
    });
//
    connect(actionAsc,&QAction::triggered,this,[=](){
        getMyFileCount(Asc);
    });
    connect(actionDesc,&QAction::triggered,this,[=](){
        getMyFileCount(Desc);
    });
    connect(actionRefresh,&QAction::triggered,this,[=](){
        getMyFileCount();

    });
    connect(actionUpload,&QAction::triggered,this,[=](){
        addUploadFiles();
    });


    connect(ui->listWidget, &QListWidget::itemDoubleClicked,
                this, [=](QListWidgetItem* item){
            if (item->text() == QStringLiteral("上传文件")) {
                // 弹出文件对话框，添加上传任务
                addUploadFiles();
            }
        });
}

void MyFileWidget::getMyFileCount(MyFileDisplay cmd){



    QNetworkRequest request;
    QString ip=m_common->getConfValue("web_server","ip");
    QString port=m_common->getConfValue("web_server","port");

    QString url=QString("http://%1:%2/myfiles/count").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));

    QJsonObject paramsObj;
    paramsObj.insert("user",m_loginInfo->getUser());
    paramsObj.insert("token",m_loginInfo->getToken());
    QJsonDocument doc(paramsObj);

    QByteArray data=doc.toJson();
    QNetworkReply *reply=manager->post(request,data);

    //读取服务器返回的数据
    connect(reply,&QNetworkReply::readyRead,this,[=](){

        QByteArray data=reply->readAll();
        qDebug()<<"return data："<<QString(data);


        QStringList list= NetworkData::getFileCount(data);
        if(!list.isEmpty()){
            QString code=list.at(0);
            if(code=="110"){

                m_myFilesCount =list.at(1).toInt();
                qDebug()<<"num:"<<m_myFilesCount;
            }else if(code=="111"){
                QMessageBox::critical(this,"账号异常","请重新登录");
                emit sigLoginAgain();
                return;
            }

        }

if(m_myFilesCount>0){
    getMyFileList(cmd);
}else{
    //文件为0
}




     reply->deleteLater();


      });//connect(reply,&QNetworkReply::readyRead
}

void MyFileWidget::getMyFileList(MyFileDisplay cmd){
    QString strCmd;
    if(cmd==MyFileDisplay::Asc){
        strCmd="asc";
    }else if(cmd==MyFileDisplay::Desc){
        strCmd="desc";
    }else strCmd="desc";

    QNetworkRequest request;
    QString ip=m_common->getConfValue("web_server","ip");
    QString port=m_common->getConfValue("web_server","port");

    //http:../myfiles?cmd=normal  -->/myfiles/list

    QString url =QString("http://%1:%2/myfiles/list").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));

/*
{
"count":1,
"start":0,
"token":"              ",
"user":"q"
}
*/
    QJsonObject paramsObj;
    paramsObj.insert("user",m_loginInfo->getUser());
    paramsObj.insert("token",m_loginInfo->getToken());
    paramsObj.insert("start",0);
    paramsObj.insert("count",m_myFilesCount);
    paramsObj.insert("order",strCmd);
    QJsonDocument doc(paramsObj);

    QByteArray data=doc.toJson();
    QNetworkReply *reply =manager->post(request,data);

    //读取服务器返回的数据
    connect(reply,&QNetworkReply::readyRead,this,[=](){
        //du
        QByteArray data=reply->readAll();
        reply->deleteLater();


        clearItems();
        clearFileList();

        m_FileList= NetworkData::getFileInfo(data);


        showFileItems();
        addUploadItem();

        reply->deleteLater();
    });
}

void MyFileWidget::addDownloadFiles(){
    emit gotoTransmission(TransmitStatus::Work);
    QListWidgetItem *item=ui->listWidget->currentItem();
    if(item==NULL){
        qDebug()<<"item==NULL";
        return;
    }
    FileInfo *fileInfo=NULL;

    int size=m_FileList.size();
    for(int i=0;i<size;i++){
        fileInfo=m_FileList.at(i);
        if(fileInfo!=NULL){
            if(fileInfo->fileName==item->text()){
                //找到fileInfo对象了
                //打开保存文件框
                QString filePath=QFileDialog::getSaveFileName(this,"请选择保存文件路径",item->text());
                qDebug()<<"filePath:"<<filePath;
                if(filePath.isEmpty()){
                    qDebug()<<"empty";
                    return;
                }

                //添加到下载任务列表
                int res=m_downloadTask->appendDownloadTask(fileInfo,filePath);
                if(res==-2){
                    m_common->writeRecord(LoginInfoInstance::getInstance()->getUser(),
                                          fileInfo->fileName,
                                          "091");
                }
                else
                if (!m_timerForDownload.isActive())
                       m_timerForDownload.start(500);
            }
        }
    }
}

void MyFileWidget::downloadFile(){//通过游览器下载文件的方式
    QListWidgetItem* item = ui->listWidget->currentItem();
     if (!item) {
         QMessageBox::warning(this, "提示", "请先选中一个文件！");
         return;
     }

     // 根据 item 获取对应的 fileInfo
     int index = ui->listWidget->row(item);
     if (index < 0 || index >= m_FileList.length()) {
         QMessageBox::warning(this, "错误", "文件索引错误");
         return;
     }

     FileInfo* fileInfo = m_FileList.at(index);
     QString url = fileInfo->url;

     if (url.isEmpty()) {
         QMessageBox::warning(this, "错误", "文件 URL 为空，无法下载！");
         return;
     }


     QDesktopServices::openUrl(QUrl(url));
}

void MyFileWidget::addUploadItem(){
    QString filePath=QString(":/Resource/2.jpg");
    ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath),QString("上传文件")));

}

void MyFileWidget::clearItems(){
    int count=ui->listWidget->count();
    for(int i=0;i<count;i++){
        QListWidgetItem *item=ui->listWidget->takeItem(0);
        delete item;
    }
}

void MyFileWidget::clearFileList(){
    int size=m_FileList.size();
    for(int i=0;i<size;i++){
        FileInfo *temp=m_FileList.takeFirst();
        if(temp!=nullptr){
            delete temp;
        }
    }
}

void MyFileWidget:: dealFile(QString cmd){//作为分享、删除函数的载体
    QListWidgetItem *item=ui->listWidget->currentItem();
    if(item==NULL){
        qDebug()<<"item NULL";
        return;
    }

    //根据item拿到fileinfo*
    for(int i=0;i<m_FileList.length();i++){
        FileInfo *fileInfo=m_FileList.at(i);

        if(fileInfo->fileName==item->text()){
            if(cmd=="share")
            shareFile(fileInfo);
            else if(cmd=="delete")
            deleteFile(fileInfo);
            else if(cmd=="property")
            showFileProperty(fileInfo);

            break;
        }
    }
}

void MyFileWidget::shareFile(FileInfo *fileinfo){
QString op = (fileinfo->shareStatus==1 ? "取消分享" : "分享");

    QNetworkRequest request;
    QString ip=m_common->getConfValue("web_server","ip");
    QString port=m_common->getConfValue("web_server","port");
    //http://.../dealfile?cmd=share  -->
    QString url=QString("http://%1:%2/dealfile/toggle_share").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
/*
示例：
{
"filename":"Makefile",
"md5":"????",
"user":"hhh"
}

*/
    QJsonObject paramsObj;
    paramsObj.insert("filename",fileinfo->fileName);
    paramsObj.insert("md5",fileinfo->md5);
    paramsObj.insert("user",m_loginInfo->getUser());
    QJsonDocument doc(paramsObj);

    QByteArray data=doc.toJson();
    QNetworkReply *reply=manager->post(request,data);

    //读取服务器返回的数据
    connect(reply,&QNetworkReply::finished,this,[=](){

    QByteArray data=reply->readAll();
    qDebug()<<"return data："<<QString(data);

    QString code= NetworkData::getCode(data);

    /*
    010:成功
    011：失败
    012：别人已经分享过此文件
    013：token验证失败
    */
    if(code=="010"){
    fileinfo->shareStatus=1;
    QMessageBox::information(this,op,"分享成功");
    }
    else if(code=="011"){
    QMessageBox::warning(this,op,"文件不存在");
    }
    else if(code=="012"){
    fileinfo->shareStatus=0;
    QMessageBox::information(this,op,"取消分享成功");
    }
//    else if(code=="013"){
//        QMessageBox::critical(this,"账号异常","请重新登录");
//        emit sigLoginAgain();
//        return;
//    }


    reply->deleteLater();

      });
}



void MyFileWidget::deleteFile(FileInfo *fileinfo){
    QNetworkRequest request;
    QString ip=m_common->getConfValue("web_server","ip");
    QString port=m_common->getConfValue("web_server","port");
    //http://.../dealfile?cmd=del  gai
    QString url=QString("http://%1:%2/dealfile/del").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
/*
示例：
{
"filename":"Makefile",
"md5":"????",
"token":"?????????",
"user":"hhh"
}

*/
    QJsonObject paramsObj;
    paramsObj.insert("filename",fileinfo->fileName);
    paramsObj.insert("md5",fileinfo->md5);
    paramsObj.insert("user",m_loginInfo->getUser());
    QJsonDocument doc(paramsObj);

    QByteArray data=doc.toJson();
    QNetworkReply *reply=manager->post(request,data);

    //读取服务器返回的数据
    connect(reply,&QNetworkReply::readyRead,this,[=](){

    QByteArray data=reply->readAll();
    qDebug()<<"return data："<<QString(data);

    QString code= NetworkData::getCode(data);

    /*
    013:成功
    014：失败
    */
    if(code=="013"){
    QMessageBox::information(this,"delete sucessfully",fileinfo->fileName);

    for(int i=0;i<m_FileList.size();i++){
        if(m_FileList.at(i)->fileName==fileinfo->fileName){
            //获取对应的listWidget

            for(int k=0;k<ui->listWidget->count();k++){
                QListWidgetItem *item=ui->listWidget->item(k);
                if(item->text()==fileinfo->fileName){
                    //delete icon
                    ui->listWidget->removeItemWidget(item);
                    delete item;
                    break;
                }
            }

            m_FileList.removeAt(i);
            delete fileinfo;
            break;
        }
    }
    }
    else if(code=="014"){
        QMessageBox::critical(this,"delete failed",fileinfo->fileName);

    }
      });
}

void MyFileWidget::showFileProperty(FileInfo *fileInfo){
    FilePropertyInfoDialog dlg;
    dlg.setFileInfo(fileInfo);
    dlg.exec();
}



void MyFileWidget::addUploadFiles(){
    emit gotoTransmission(TransmitStatus::Work);

    QStringList filePathlist=QFileDialog::getOpenFileNames();
    if(filePathlist.isEmpty())return;
    for(auto &fp:filePathlist)
    {
        int res=m_uploadTask->appendUploadTask(fp);
        if(res==-1){
            QMessageBox::warning(this,"warning","超出大小限制");
            return;
        }

    }
    if(!m_timer.isActive())
        m_timer.start(500);
}




void MyFileWidget::uploadFile(UploadFileInfo* info) {
    // 打开文件
    QFile *file = new QFile(info->filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        file->close();
        return;
    }

    if (info->md5.isEmpty()) {
           // 实现 getFileMd5() 函数计算文件 MD5
           info->md5 = Common::getInstance()->getFileMd5(info->filePath);
       }

    auto *multi = new QHttpMultiPart(QHttpMultiPart::FormDataType);
     QHttpPart part;
     part.setHeader(QNetworkRequest::ContentDispositionHeader,
         QVariant("form-data; name=\"file\"; filename=\"" + info->fileName + "\""));
     part.setBodyDevice(file);
     file->setParent(multi);
     multi->append(part);

     // 额外字段
     QHttpPart userPart;
     userPart.setHeader(QNetworkRequest::ContentDispositionHeader,
         "form-data; name=\"user\"");
     userPart.setBody(m_loginInfo->getUser().toUtf8());
     multi->append(userPart);


     // md5 part
     QHttpPart md5Part;
     md5Part.setHeader(QNetworkRequest::ContentDispositionHeader,
         "form-data; name=\"md5\"");
     md5Part.setBody(info->md5.toUtf8());
     multi->append(md5Part);


     QString ip=m_common->getConfValue("web_server","ip");
     QString port=m_common->getConfValue("web_server","port");

//     QString url=QString("http://%1:%2/...").arg(ip).arg(port);

     QNetworkRequest req(QUrl(
         QString("http://%1:%2/upload").arg(ip).arg(port)
     ));
     QNetworkReply *reply = manager->post(req, multi);
     multi->setParent(reply);


     //显示进度条
//     connect(reply,&QNetworkReply::uploadProgress,this,[=](qint64 bytesSent,qint64 bytesTotal){
//         //bytesSent上传字节数 bytesTotal总字节数


//         info->fileProgress->setProgress(bytesSent/1024,bytesTotal/1024);
//     });



     connect(reply, &QNetworkReply::finished, this, [=]() {
         QByteArray resp =reply->readAll();
         QString code =NetworkData::getCode(resp);

         if (code == "008") {
             info->UploadStatus=UPLOAD_FINISHED;
             QMessageBox::information(this, "success", info->fileName);
             m_common->writeRecord(LoginInfoInstance::getInstance()->getUser(),
                                   info->fileName,
                                   "008");

         } else {
             info->UploadStatus=UPLOAD_FAILED;
             QMessageBox::warning(this, "error", info->fileName);
             m_common->writeRecord(LoginInfoInstance::getInstance()->getUser(),
                               info->fileName,
                               "009");
         }

         m_uploadTask->delUploadTask();
         getMyFileCount();
         file->close();
         reply->deleteLater();
     });

}


void MyFileWidget::checkTaskList(){
    connect(&m_timer,&QTimer::timeout,this,[=](){
        if (m_uploadTask->isEmpty()) {
                    m_timer.stop();
                } else {
                    uploadFileAction();
                }
    });
    //上传
    m_timer.start(500);



}

void MyFileWidget::uploadFileAction(){
    if(m_uploadTask->isEmpty()){
        return;
    }

    UploadFileInfo *uploadFileInfo=m_uploadTask->takeTask();
    if(!uploadFileInfo)return;
    uploadFileInfo->UploadStatus=UPLOAD_IN_PROGRESS;

    QNetworkRequest request;
    QString ip=m_common->getConfValue("web_server","ip");
    QString port=m_common->getConfValue("web_server","port");
//http://111.111.11.111/md5
    QString url=QString("http://%1:%2/md5").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));

    QJsonObject paramsObj;
    paramsObj.insert("user",m_loginInfo->getUser());
    paramsObj.insert("token",m_loginInfo->getToken());
    paramsObj.insert("md5",uploadFileInfo->md5);
    paramsObj.insert("filename",uploadFileInfo->fileName);
    QJsonDocument doc(paramsObj);

    QByteArray data=doc.toJson();
    QNetworkReply *reply=manager->post(request,data);

    //读取服务器返回的数据
    connect(reply,&QNetworkReply::finished,this,[=](){

        QByteArray data=reply->readAll();
        qDebug()<<"return data："<<QString(data);


        QString code= NetworkData::getCode(data);
/*
005 文件已存在
006 成功
007 失败
111 token验证失败
*/
        if(code=="007"){
        //没有此文件，进行上传操作
        uploadFile(uploadFileInfo);
        m_common->writeRecord(LoginInfoInstance::getInstance()->getUser(),
                              uploadFileInfo->fileName,
                              "007");
        }
        else if(code=="111"){
            QMessageBox::critical(this,"账号异常","请重新登录");
            emit sigLoginAgain();
            return;
        }
        else {
               uploadFileInfo->UploadStatus=(code=="005")?UPLOAD_FINISHED:UPLOAD_FAILED;
               m_uploadTask->delUploadTask();
               m_common->writeRecord(LoginInfoInstance::getInstance()->getUser(),
                                     uploadFileInfo->fileName,
                                     "006");
        }


     reply->deleteLater();
      });
}

void MyFileWidget::downloadFileAction(){
    if (m_downloadTask->isEmpty()) {
           m_timerForDownload.stop();
           return;
       }

    if(m_downloadTask->isEmpty()){
        return;
    }
    DownloadFileInfo *downloadFileInfo=m_downloadTask->takeTask();
    if(downloadFileInfo==NULL){
        return;
    }

    QFile *file=downloadFileInfo->file;
    // 如果文件还没打开，就打开（并清空旧内容）
    if (!file->isOpen()) {
        if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QMessageBox::warning(this, "错误", "无法打开本地文件写入");
            m_downloadTask->delDownloadTask();
            return;
        }
    }

    QUrl url=QUrl(downloadFileInfo->url);
    QNetworkRequest request;
    request.setUrl(url);

    QNetworkReply *reply=manager->get(request);
    if(reply==NULL){
        //删除任务
        m_downloadTask->delDownloadTask();
        qDebug()<<"下载文件失败";
        return;
    }


    connect(reply,&QNetworkReply::uploadProgress,this,[=](qint64 bytesSent,qint64 bytesTotal){
        //bytesSent上传字节数 bytesTotal总字节数

        downloadFileInfo->fileProgress->setProgress(bytesSent/1024,bytesTotal/1024);
    });
    connect(reply,&QNetworkReply::readyRead,[=](){
        file->write(reply->readAll());
    });
    connect(reply,&QNetworkReply::finished,[=](){


      file->flush();
      file->close();

      m_common->writeRecord(LoginInfoInstance::getInstance()->getUser(),
                            downloadFileInfo->fileName,
                            "090");
    reply->deleteLater();

    m_downloadTask->delDownloadTask();
    });
}

void MyFileWidget::getShareFileList() {
    QNetworkRequest request;
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");
    QString url = QString("http://%1:%2/sharelist").arg(ip).arg(port);

    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json.insert("user", m_loginInfo->getUser());

    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();
        qDebug() << "共享中心返回数据:" << data;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError) {
            qDebug() << "JSON 解析失败";
            return;
        }

        QJsonObject obj = doc.object();
        QString code = obj.value("code").toString();
        if (code == "120") {
            m_FileList.clear();
            QJsonArray files = obj.value("files").toArray();

            for (const QJsonValue &v : files) {
                QJsonObject f = v.toObject();
                FileInfo *info = new FileInfo;
                info->fileName = f.value("file_name").toString();
                info->user = f.value("user_name").toString();
                info->url = f.value("url").toString();
                info->type = f.value("type").toString();
                info->size = f.value("size").toInt();
                info->pv = f.value("pv").toInt();
                info->shareStatus = f.value("share_status").toInt();
                info->createTime = f.value("create_time").toString();

                m_FileList.append(info);
            }

            showFileItems(); // 渲染共享文件
        }
        reply->deleteLater();
    });
}



void MyFileWidget::showFileItems(){

    ui->listWidget->clear();

    for(int i=0;i<m_FileList.length();i++){
       FileInfo *fileInfo=m_FileList.at(i);
       QString fileType = fileInfo->type.toLower();
       //图标
       QString filePath=QString(":/Resource/TypeIcon/%1.png").arg(fileType);
       QPixmap pix(filePath);

       QFileInfo qfileInfo(filePath);
       QIcon icon;
       if (!pix.isNull()) {
               icon = QIcon(pix);
               } else {    // 加载失败就使用 unknown.png
           icon = QIcon(":/Resource/TypeIcon/unknown.png");
           qDebug() << "icon does not exist, fallback to unknown:" << filePath;
       }

       // 添加到 listWidget
       QListWidgetItem *item = new QListWidgetItem(icon, fileInfo->fileName);
       ui->listWidget->addItem(item);


    }



}


