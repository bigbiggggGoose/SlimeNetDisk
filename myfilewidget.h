#ifndef MYFILEWIDGET_H
#define MYFILEWIDGET_H
#include"mymenu.h"
#include"common.h"
#include"logininfoinstance.h"
#include <QWidget>
#include<QAction>
#include"networkdata.h"
#include"uploadtask.h"
#include"downloadtask.h"
#include<QTimer>
namespace Ui {
class MyFileWidget;
}

class MyFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyFileWidget(QWidget *parent = nullptr);
    ~MyFileWidget();

private:
    enum MyFileDisplay {
        Normal,
        Asc,
        Desc
    };

    Ui::MyFileWidget *ui;
    MyMenu *m_inmenu;
    MyMenu *m_exmenu;
    int m_myFilesCount;
    Common *m_common;
    LoginInfoInstance *m_loginInfo;
    QNetworkAccessManager *manager;
    QList<FileInfo*>  m_FileList;
    QStringList m_FileTypeList;
    UploadTask * m_uploadTask;
    DownloadTask * m_downloadTask;

    QTimer m_timer;
    QTimer m_timerForDownload;


    QAction *actionDownload;
    QAction *actionShare;
    QAction *actionDelete;
    QAction *actionProperty;
    QAction *actionAsc;
    QAction *actionDesc;
    QAction *actionRefresh;
    QAction *actionUpload;
    QAction *actionRename;

    void initListWidget();
    void addMenu();
    void menuActions();
    void getMyFileCount(MyFileDisplay cmd=Normal);
    void getMyFileList(MyFileDisplay cmd=Normal);
    void showFileItems();
    void addUploadItem();
    void clearItems();
    void dealFile(QString cmd);
    void clearFileList();
    void shareFile(FileInfo *fileinfo);
    void deleteFile(FileInfo *fileInfo);
    void renameFile(FileInfo *fileInfo);
    void addUploadFiles();
    void uploadFile(UploadFileInfo* info);
    void showFileProperty(FileInfo *fileInfo);
    void getStrMd5();
    void checkTaskList();
    void uploadFileAction();
    void downloadFileAction();
    void getShareFileList();
    void downloadFile();
    void addDownloadFiles();


signals:
    void sigLoginAgain();
    void gotoTransmission(TransmitStatus status);//TransmitStatus放在common
private slots:
    void onRightMenu(const QPoint &pos);
};

#endif // MYFILEWIDGET_H
