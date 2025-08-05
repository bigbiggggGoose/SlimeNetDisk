#include "filepropertyinfodialog.h"
#include "ui_filepropertyinfodialog.h"
#include<QDesktopServices>
FilePropertyInfoDialog::FilePropertyInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilePropertyInfoDialog)
{
    ui->setupUi(this);
    ui->val_download->setOpenExternalLinks(1);//定位超链接

    QString style1=QString("color: rgb(11,11,11);font:75 14pt \"Agency FB\";");
    QString style2=QString("font: 14pt;color: rgb(255,156,16);");
    ui->val_fileName->setStyleSheet(style2);
    ui->val_fileSize->setStyleSheet(style2);
    ui->val_download->setStyleSheet(style2);
    ui->val_shareStatus->setStyleSheet(style2);
    ui->val_uploadTime->setStyleSheet(style2);
    ui->val_uploadUser->setStyleSheet(style2);
    ui->val_pv->setStyleSheet(style2);

}

FilePropertyInfoDialog::~FilePropertyInfoDialog()
{
    delete ui;
}

void FilePropertyInfoDialog::setFileInfo(FileInfo *fileInfo){
    qDebug() << "File Info:";
       qDebug() << "  Name:" << fileInfo->fileName;
       qDebug() << "  Size:" << fileInfo->size;
       qDebug() << "  User:" << fileInfo->user;  // 检查用户字段

    ui->val_fileName->setText(fileInfo->fileName);

    int size=fileInfo->size;
    if(size>=1024&&size<1024*1024){
    ui->val_fileSize->setText(QString("%1 KB").arg(size/1024.0));
    }
    else{
    ui->val_fileSize->setText(QString("%1 MB").arg(size/1024.0/1024.0));
    }


    ui->val_pv->setText(QString("被下载 %1 次").arg(fileInfo->pv));
    if(fileInfo->shareStatus==0){
        ui->val_shareStatus->setText("未分享");
    }else{
        ui->val_shareStatus->setText("已分享");
    }
    QString tmp = QString("<a href=\"%1\">下载地址</a>").arg(fileInfo->url);
    ui->val_uploadUser->setText(QString("%1").arg(fileInfo->user));
    ui->val_download->setText(tmp);
    ui->val_uploadTime->setText(fileInfo->createTime);
    ui->val_uploadUser->setText(fileInfo->user);

    ui->val_download->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->val_download->setOpenExternalLinks(true);

    connect(ui->val_download, &QLabel::linkActivated,
            this, &FilePropertyInfoDialog::on_val_download_linkActivated);
}

void FilePropertyInfoDialog::on_val_download_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}
