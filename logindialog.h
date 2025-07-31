#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include"common.h"
#include"mainwindow.h"



namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

protected:
    void paintEvent(QPaintEvent *event);


private slots:
    void on_close_clicked();

    void on_min_clicked();

    void on_login_button_clicked();

    void on_reg_button_clicked();

    void on_setting_clicked();

    void on_setting_button_clicked();

    void on_reg_button_2_clicked();

    private:
    Ui::LoginDialog *ui;
    MainWindow *m_mainwindow;
    Common* m_common;
    void readConf();
    void saveLoginInfoData(QString username,QString token,QString ip,QString port);
};

#endif // LOGINDIALOG_H
