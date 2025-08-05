#ifndef SHAREWIDGET_H
#define SHAREWIDGET_H
#include"mymenu.h"
#include"common.h"
#include"logininfoinstance.h"
#include <QWidget>
#include<QAction>
#include"networkdata.h"
#include"uploadtask.h"
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
    UploadTask * m_uploadTask;
    QTimer m_timer;


    QAction *actionDownload;
    QAction *actionProperty;
    QAction *actionRefresh;


    void initListWidget();
    void addMenu();
    void menuActions();
    void clearItems();
    void dealProperty();
    void getShareFileList();
    void downloadFile();
    void showFileItems();

signals:
    void sigLoginAgain();

private slots:
    void onRightMenu(const QPoint &pos);

};

#endif // SHAREWIDGET_H
