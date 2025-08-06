#ifndef UPLOADTASK_H
#define UPLOADTASK_H
#include<QString>
#include<QList>
#include"common.h"
#include<QFileInfo>
#include"uploadprogress.h"

#define UPLOAD_WAITING  0
#define UPLOAD_IN_PROGRESS   1
#define UPLOAD_FINISHED   2
#define UPLOAD_FAILED   3

struct UploadFileInfo{
    QString md5;
    QString fileName;
    QString filePath;
    qint64 size;
    UploadProgress *fileProgress;
    int UploadStatus=UPLOAD_WAITING;
};

class UploadTask
{

    QList<UploadFileInfo*> m_fileList;
public:
    UploadTask();
    ~UploadTask();
    int appendUploadTask(QString filePath);
    bool isEmpty();
    UploadFileInfo* takeTask();
    void delUploadTask();
};

#endif // UPLOADTASK_H
