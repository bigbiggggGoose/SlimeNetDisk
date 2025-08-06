#include "downloadtask.h"
#include<QByteArray>
#include<QFileInfo>
#include<QVBoxLayout>
#include"Layout.h"
DownloadTask::DownloadTask()
{

}
DownloadTask:: ~DownloadTask(){
   for(int i=0;i<m_fileList.length();i++){
       if(m_fileList.at(i)!=nullptr) {
           // 先关闭并删除文件对象
           if(m_fileList.at(i)->file) {
               m_fileList.at(i)->file->close();
               delete m_fileList.at(i)->file;
           }
           // 删除进度条
           if(m_fileList.at(i)->fileProgress) {
               delete m_fileList.at(i)->fileProgress;
           }
           // 删除DownloadFileInfo对象
           delete m_fileList.at(i);
       }
   }

}


int DownloadTask::appendDownloadTask(FileInfo* fileInfo, QString filePath){


    QFile *file=new QFile(filePath);
    if(!file->open(QIODevice::WriteOnly)){
        qDebug()<<"文件打开失败";
        delete file;
        file=NULL;
        return -2;
    }
    DownloadFileInfo *task=new DownloadFileInfo;

    //截取字符串
    int pos=filePath.lastIndexOf("/",-1)+1;
    task->user=fileInfo->user;
    task->fileName=filePath.mid(pos);
    task->filePath=filePath;

    task->url=fileInfo->url;
    task->file=file;

    //进度条
    UploadProgress *progress=new UploadProgress();
    progress->setFileName(task->fileName);
    task->fileProgress=progress;

    QVBoxLayout *vLayout=Layout::getInstance()->getLayout();
    vLayout->insertWidget(0,progress);
    m_fileList.append(task);

    return 0;
}

bool DownloadTask::isEmpty(){
   return m_fileList.isEmpty();
}

DownloadFileInfo* DownloadTask::takeTask(){
    DownloadFileInfo* temp=NULL;
    if(m_fileList.size()>0){
        temp=m_fileList.at(0);

    }

    return temp;
}

void DownloadTask::delDownloadTask(){
    //1进度条从vLayout中移除
    //2删除进度条
    //3删除UploadFileInfo
    for(int i=0;i<m_fileList.size();i++){
    DownloadFileInfo* temp=m_fileList.at(i);
    DownloadFileInfo *DownloadFileInfo=m_fileList.takeAt(0);
    if(DownloadFileInfo!=NULL){
        QVBoxLayout *vLayout=Layout::getInstance()->getLayout();
        vLayout->removeWidget(DownloadFileInfo->fileProgress);

        delete DownloadFileInfo->fileProgress;

        QFile *file=DownloadFileInfo->file;
        if(file) {
                file->close();
                delete file;
            }

        delete DownloadFileInfo;
    }
  }

}
