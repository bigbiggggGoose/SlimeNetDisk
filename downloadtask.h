#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include<QString>
#include<QList>
#include"common.h"
#include<QFileInfo>
#include"uploadprogress.h"
#include"networkdata.h"
#include<QFile>

#define DOWNLOAD_NOT  0
#define DOWNLOADING   1
#define DOWNLOAD_FINISHED   2
#define DOWNLOAD_FAILED   3

struct DownloadFileInfo{
    QFile *file;  //文件
    QString user; //下载人
    QString md5;
    QString fileName;
    QString filePath;
    qint64 size;
    UploadProgress *fileProgress;
    int DownloadStatus;
    QUrl url;
};
class DownloadTask
{
    DownloadTask *p;
    QList<DownloadFileInfo*> m_fileList;
public:
    DownloadTask();
    ~DownloadTask();
    DownloadTask(DownloadTask*a);
    DownloadTask& operator=(const DownloadTask&a);
    int appendDownloadTask(FileInfo* fileInfo,QString filePath);
    bool isEmpty();
    DownloadFileInfo* takeTask();
    void delDownloadTask();
};

#endif // DOWNLOADTASK_H
