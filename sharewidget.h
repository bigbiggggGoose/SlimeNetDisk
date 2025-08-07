#ifndef SHAREWIDGET_H
#define SHAREWIDGET_H
#include"mymenu.h"
#include"common.h"
#include"logininfoinstance.h"
#include <QWidget>
#include<QAction>
#include"networkdata.h"
#include"downloadtask.h"
#include<QTimer>
namespace Ui {
class ShareWidget;
}

class ShareWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ShareWidget(QWidget *parent = nullptr);
    ~ShareWidget();

private:


    Ui::ShareWidget *ui;

    MyMenu *m_inmenu;
    MyMenu *m_exmenu;
    int m_myFilesCount;
    Common *m_common;
    LoginInfoInstance *m_loginInfo;
    QNetworkAccessManager *manager;
    QList<FileInfo*>  m_FileList;
    QStringList m_FileTypeList;
    DownloadTask * m_downloadTask;
    QTimer m_timerForDownload;


    QAction *actionDownload;
    QAction *actionProperty;
    QAction *actionRefresh;


    void initListWidget();
    void addMenu();
    void menuActions();
    void clearItems();
    void dealProperty();
    void getShareFileList();
    void showFileItems();
    void downloadFile();

signals:
    void sigLoginAgain();
    void gotoTransmission(TransmitStatus status);
private slots:
    void onRightMenu(const QPoint &pos);

};

#endif // SHAREWIDGET_H
