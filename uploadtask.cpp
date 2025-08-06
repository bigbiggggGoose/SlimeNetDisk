#include "uploadtask.h"
#include<QByteArray>
#include<QFileInfo>
#include<QVBoxLayout>
#include"Layout.h"


UploadTask::UploadTask()
{

}

UploadTask:: ~UploadTask(){
   for(int i=0;i<m_fileList.length();i++){
       if(m_fileList.at(i)!=nullptr)
       delete m_fileList.at(i);
   }

}
/*
UploadTask::UploadTask(UploadTask*a){//深拷贝1
    p=new UploadTask(a->p);
}
UploadTask& UploadTask::operator=(const UploadTask&a){
    if (this != &a) {//防止自赋值
        this->p=a.p;
    }
    return  *this;
}
*/
int UploadTask::appendUploadTask(QString filePath){
    QFileInfo fileInfo(filePath);
    qint64 fileSize=fileInfo.size();
    if(fileSize>40*1024*1024){
    qDebug()<<"文件太大，max:40M";
    return -1;
    }

    UploadFileInfo *uploadFile=new UploadFileInfo;

    //截取字符串
//    int pos=filePath.lastIndexOf("/",-1)+1;//从字符串末尾开始查找最后一个"/"的位置 +1跳过斜杠
//    uploadFile->fileName=filePath.mid(pos);//mid从pos截取到末尾
    uploadFile->fileName = fileInfo.fileName();//这里改用qfileinfo获取filename
    uploadFile->filePath=filePath;
    uploadFile->size=fileSize;
    uploadFile->md5=Common::getInstance()->getFileMd5(filePath);
    //进度条
    UploadProgress *progress=new UploadProgress();
    progress->setFileName(uploadFile->fileName);
    uploadFile->fileProgress=progress;

    QVBoxLayout *vLayout=Layout::getInstance()->getLayout();
    vLayout->insertWidget(0,progress);



    m_fileList.append(uploadFile);

    return 0;
}

bool UploadTask::isEmpty(){
   return m_fileList.isEmpty();
}

UploadFileInfo* UploadTask::takeTask(){
    UploadFileInfo* temp=NULL;
    if(m_fileList.size()>0){
        if(m_fileList[0]->UploadStatus==UPLOAD_WAITING){
        temp=m_fileList.at(0);
        return temp;
        }

    }
    return NULL;

}

void UploadTask::delUploadTask(){
    //1进度条从vLayout中移除
    //2删除进度条
    //3删除UploadFileInfo
    for(int i=0;i<m_fileList.size();i++){
    UploadFileInfo* temp=m_fileList.at(i);
    if(temp->UploadStatus==UPLOAD_FINISHED||temp->UploadStatus==UPLOAD_FAILED){
    UploadFileInfo *uploadFileInfo=m_fileList.takeAt(0);
    if(uploadFileInfo!=NULL){
        QVBoxLayout *vLayout=Layout::getInstance()->getLayout();
        vLayout->removeWidget(uploadFileInfo->fileProgress);

        delete uploadFileInfo->fileProgress;

        delete uploadFileInfo;
    }
  }
    }
}


