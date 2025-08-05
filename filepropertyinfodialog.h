#ifndef FILEPROPERTYINFODIALOG_H
#define FILEPROPERTYINFODIALOG_H

#include <QDialog>
#include<QFileInfo>
#include"networkdata.h"

namespace Ui {
class FilePropertyInfoDialog;
}

class FilePropertyInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePropertyInfoDialog(QWidget *parent = nullptr);
    ~FilePropertyInfoDialog();
    void setFileInfo(FileInfo *fileInfo);
private slots:
    void on_val_download_linkActivated(const QString &link);

private:
    Ui::FilePropertyInfoDialog *ui;


};

#endif // FILEPROPERTYINFODIALOG_H
