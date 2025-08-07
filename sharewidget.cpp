#include "sharewidget.h"
#include "ui_sharewidget.h"
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

ShareWidget::ShareWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareWidget)
{
    ui->setupUi(this);


    this->setWindowFlags(Qt::FramelessWindowHint|windowFlags());
    m_common=Common:: getInstance();
    m_loginInfo=LoginInfoInstance::getInstance();
    manager=m_common->getNetworkAccessManager();

    initListWidget();



    addMenu();
    menuActions();
    getShareFileList();



}
ShareWidget::~ShareWidget()
{
    m_timerForDownload.stop();
    if(m_downloadTask) {
        delete m_downloadTask;
        m_downloadTask = nullptr;
    }

    // 清理菜单对象
    if(m_inmenu) {
        delete m_inmenu;
        m_inmenu = nullptr;
    }
    if(m_exmenu) {
        delete m_exmenu;
        m_exmenu = nullptr;
    }
    delete ui;
}

void ShareWidget::clearItems(){
    ui->listWidget->clear();
}



void ShareWidget::initListWidget(){
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(80,80));
    ui->listWidget->setGridSize(QSize(100,200));


    ui->listWidget->setResizeMode(QListView::Adjust);

    //menu
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget,&QListWidget::customContextMenuRequested,this,&ShareWidget::onRightMenu);
}
void ShareWidget::onRightMenu(const QPoint &pos){
    QListWidgetItem *item=ui->listWidget->itemAt(pos);
    if(item){

        m_inmenu->exec(QCursor::pos());
    }else{

        m_exmenu->exec(QCursor::pos());
    }
}

void ShareWidget::addMenu(){
    m_inmenu=new MyMenu(this);

    actionDownload=new QAction("下载",this);
    actionProperty=new QAction("属性",this);
    m_inmenu->addAction(actionDownload);
    m_inmenu->addAction(actionProperty);

    //外菜单：
    m_exmenu=new MyMenu(this);

    actionRefresh=new QAction("刷新",this);
    m_exmenu->addAction(actionRefresh);
}


void ShareWidget::menuActions(){
    connect(actionDownload,&QAction::triggered,this,[=](){
       downloadFile();
    });

    connect(actionProperty,&QAction::triggered,this,[=](){
        dealProperty();
    });

    connect(actionRefresh,&QAction::triggered,this,[=](){
        getShareFileList();

    });

}


void ShareWidget::getShareFileList() {
    clearItems();
    qDeleteAll(m_FileList);
    m_FileList.clear();

    QNetworkRequest request;
    QString ip = m_common->getConfValue("web_server", "ip");
    QString port = m_common->getConfValue("web_server", "port");
    QString url = QString("http://%1:%2/sharelist").arg(ip).arg(port);

    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json.insert("user", m_loginInfo->getUser());
    json.insert("token", m_loginInfo->getToken());

    QNetworkReply *reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray data = reply->readAll();


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
                info->user = f.value("user").toString();
                info->url = f.value("url").toString();
                info->type = f.value("type").toString();
                info->size = f.value("size").toInt();
                info->pv = f.value("pv").toInt();
                info->shareStatus = f.value("share_status").toInt();
                info->createTime = f.value("create_time").toString();

                m_FileList.append(info);
            }

            showFileItems();
        }else if (code == "111") {
            QMessageBox::critical(this, "账号异常", "请重新登录");
            emit sigLoginAgain();
        }

    });
}


void ShareWidget::showFileItems(){

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
               } else {
           icon = QIcon(":/Resource/TypeIcon/unknown.png");
       }


       QListWidgetItem *item = new QListWidgetItem(icon, fileInfo->fileName);
       ui->listWidget->addItem(item);


    }
  }
    void ShareWidget::dealProperty(){
        QListWidgetItem *item=ui->listWidget->currentItem();
        if(item==NULL){
            qDebug()<<"item NULL";
            return;
        }

        //根据item拿到fileinfo*
        for(int i=0;i<m_FileList.length();i++){
            FileInfo *fileInfo=m_FileList.at(i);
            if(fileInfo->fileName==item->text()){

                FilePropertyInfoDialog dlg;
                dlg.setFileInfo(fileInfo);
                dlg.exec();

                break;
            }
        }
    }

    void ShareWidget::downloadFile(){
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

            qDebug() << "正在打开下载链接：" << url;
            QDesktopServices::openUrl(QUrl(url));
       }












