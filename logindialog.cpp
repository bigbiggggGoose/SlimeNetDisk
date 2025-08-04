#include "logindialog.h"
#include "ui_logindialog.h"
#include "ui_mainwindow.h"
#include<QPainter>
#include <QMessageBox>
#include<QDebug>
#include<QRegExp>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include<QNetworkRequest>
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonValue>
#include"logininfoinstance.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
this->show();
    //去掉标题栏
    this->setWindowFlags(Qt::FramelessWindowHint|windowFlags());

    ui->logo->setPixmap(QPixmap(":/Resource/slm_party.png").scaled(40,40));

    //stacked widget设置默认widget
    ui->stackedWidget->setCurrentIndex(0);

    m_mainwindow=new MainWindow;



    m_common=Common::getInstance();

    readConf();

    connect(m_mainwindow,&MainWindow::sigChangeuser,this, [=](){
    this->show();
    m_mainwindow->hide();
    });

    connect(m_mainwindow,&MainWindow::sigLoginAgain,this, [=](){
    this->show();
    m_mainwindow->hide();
    });

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::paintEvent(QPaintEvent *event){//背景
    QPainter p(this);

    QPixmap bg(":/Resource/slm.jpg");
    p.drawPixmap(0,0,this->width(),this->height(),bg);

}

//
void LoginDialog::readConf(){
    QString user =m_common->getConfValue("login","user");
    QString pwd =m_common->getConfValue("login","pwd");
    QString remember =m_common->getConfValue("login","remember");

    QString ip=m_common->getConfValue("web_server","ip");
    QString port=m_common->getConfValue("web_server","port");
    ui->server_addr->setText(ip);
    ui->server_port->setText(port);

    int ret=0;
    if(remember== "yes"){
        //1 解码base64
        QByteArray pwdTemp=QByteArray::fromBase64((pwd.toLocal8Bit()));

        unsigned char encPwd[512]={0};
        int encPwdLen=0;

        //2 des解密
        ret=DesDec((unsigned char*)pwdTemp.data(),pwdTemp.size(),encPwd,&encPwdLen);
        if(ret!=0){
            qDebug()<<"解密失败";
        }
        //将本地编码转为string
        QString password = QString::fromLocal8Bit((const char*)encPwd,encPwdLen);
        ui->password->setText(password);


        ui->checkBox1->setChecked(1);
    }



        QByteArray userTemp=QByteArray::fromBase64((user.toLocal8Bit()));

        unsigned char encUsr[512]={0};
        int encUsrLen=0;


        ret=DesDec((unsigned char*)userTemp.data(),userTemp.size(),encUsr,&encUsrLen);
        if(ret!=0){
            qDebug()<<"解密失败";
        }

        QString userName = QString::fromLocal8Bit((const char*)encUsr,encUsrLen);
        ui->username->setText(userName);


}

void LoginDialog::saveLoginInfoData(QString username,QString token,QString ip,QString port){
    LoginInfoInstance * loginInfo=LoginInfoInstance::getInstance();

    loginInfo->setUser(username);
    loginInfo->setToken(token);
    loginInfo->setIp(ip);
    loginInfo->setPort(port);
}



void LoginDialog::on_close_clicked()
{   if(ui->stackedWidget->currentIndex()==0)
    this->close();
    else ui->stackedWidget->setCurrentIndex(0);
}

void LoginDialog::on_min_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void LoginDialog::on_setting_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void LoginDialog::on_reg_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void LoginDialog::on_login_button_clicked()
{


    QString username=ui->username->text();
    QString password=ui->password->text();

    QRegExp regexp(USER_REG);
    if(!regexp.exactMatch(username)){
        QMessageBox::warning(this,"警告","用户名格式错误");
        //ui->username->clear();
        ui->username->setFocus();
        return;
    }

    regexp.setPattern(PASSWD_REG);
    if(!regexp.exactMatch(password)){
        QMessageBox::warning(this,"警告","密码格式错误");
        ui->password->clear();
        ui->password->setFocus();
        return;
    }




    //登录实现
    QNetworkAccessManager *manager=new QNetworkAccessManager (this);
    QNetworkRequest request;//栈

    //获取ip 放入
    QString ip=m_common->getConfValue("web_server","ip");
    QString port=m_common->getConfValue("web_server","port");

    QString url=QString("http://%1:%2/login").arg(ip).arg(port);//http://....../login   000成功 001失败


    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));



    QJsonObject paramsObj;

    paramsObj.insert("user",username);
    paramsObj.insert("pwd",m_common->getStrMd5(password));
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply=manager->post(request,data);

    //读取服务器返回的数据
    connect(reply,&QNetworkReply::finished,this,[=](){

            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::critical(this, "网络错误", reply->errorString());
                reply->deleteLater();
                return;
            }

        QByteArray data=reply->readAll();
        qDebug()<<"服务器返回数据："<<QString(data);
        if (data.isEmpty()) {
            QMessageBox::warning(this, "错误", "服务器返回空数据，请检查服务器状态！");
            reply->deleteLater();
            return;
        }



        QJsonParseError err;
        QJsonDocument rootDoc = QJsonDocument::fromJson(data,&err);
        if(err.error !=QJsonParseError::NoError){

            qDebug()<<"QJsonDocument error";
        }
    else{

         QJsonObject rootObj =rootDoc.object();
         QJsonValue codeValue=rootObj.value("code");
         if(codeValue.type()==QJsonValue::String){
         QString code=codeValue.toString();


        if(code=="001"){
            QMessageBox::information(this,"提示","login failed");
        }
        else if(code=="000"){


            //Common
            bool isCheck =ui->checkBox1->isChecked();
            if(!isCheck){
                ui->password->setText("");
            }

            //写入
            Common *common=Common::getInstance();
            common->writeLoginInfo(username,password,isCheck,CFG_FILE);

            //token
            QJsonValue tokenValue=rootObj.value("token");
            saveLoginInfoData(username,tokenValue.toString(),ip,port);

            //新页面
            m_mainwindow->setUser(username);

            m_mainwindow->show();
            this->hide();

        }

        }


    }
        reply->deleteLater();
    });

}

void LoginDialog::on_setting_button_clicked()
{
    QString ip=ui->server_addr->text();
    QString port=ui->server_port->text();

    QRegExp regexp(IP_REG);
    if(!regexp.exactMatch(ip)){
        QMessageBox::warning(this,"警告","服务器地址格式错误");
        //ui->server_addr->clear();
        ui->server_addr->setFocus();
        return;
    }

    regexp.setPattern(PORT_REG);
    if(!regexp.exactMatch(port)){
        QMessageBox::warning(this,"警告","服务器端口格式错误");
        //ui->server_port->clear();
        ui->server_port->setFocus();
        return;
    }
    Common *common=Common::getInstance();
    common->writeWebInfo(ip,port,CFG_FILE);
QMessageBox::information(this,"提示","success");
    //跳转
    ui->stackedWidget->setCurrentIndex(0);



}

void LoginDialog::on_reg_button_2_clicked()
{
    QString reg_name=ui->reg_name->text();
    QString reg_nickname=ui->reg_nickname->text();
    QString reg_password=ui->reg_password->text();
    QString confirm_pwd=ui->confirm_pwd->text();
    QString phone_num=ui->phone_num->text();
    QString email=ui->email->text();

    QRegExp regexp(USER_REG);
    if(!regexp.exactMatch(reg_name)){
        QMessageBox::warning(this,"警告","用户名格式错误");
        //ui->reg_name->clear();
        ui->reg_name->setFocus();
        return;
    }
    regexp.setPattern(PASSWD_REG);
    if(!regexp.exactMatch(reg_password)){
        QMessageBox::warning(this,"警告","密码格式错误");
        ui->reg_password->clear();
        ui->reg_password->setFocus();
        return;
    }

    if(reg_password!=confirm_pwd){
        QMessageBox::warning(this,"警告","错认密码错误");
    }

    regexp.setPattern(PHONE_REG);
    if(!regexp.exactMatch(phone_num)){
        QMessageBox::warning(this,"警告","电话格式错误");
        //ui->phone_num->clear();
        ui->phone_num->setFocus();
        return;
    }

    regexp.setPattern(EMAIL_REG);
    if(!regexp.exactMatch(email)){
        QMessageBox::warning(this,"警告","地址格式错误");
        //ui->email->clear();
        ui->email->setFocus();
        return;
    }

    // 1设置url
    // 2设置请求头
    // 3封装json
    // 4发送请求
    // 5读取服务器返回数据
    // 6解析返回的数据
    QNetworkAccessManager *manager = Common::getInstance()->getNetworkAccessManager();
    QNetworkRequest request;
    QString ip=m_common->getConfValue("web_server","ip");
    QString port=m_common->getConfValue("web_server","port");

    QString url=QString("http://%1:%2/reg").arg(ip).arg(port); //("http://.../reg");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));



    QJsonObject paramsObj;
    paramsObj.insert("email",email);
    paramsObj.insert("userName",reg_name);
    paramsObj.insert("phone",phone_num);
    paramsObj.insert("nickName",reg_nickname);
    paramsObj.insert("firstPwd",m_common->getStrMd5(reg_password));
    QJsonDocument doc(paramsObj);

    QByteArray data = doc.toJson();
    QNetworkReply *reply=manager->post(request,data);

    //读取服务器返回的数据
    connect(reply,&QNetworkReply::finished,this,[=](){
        QByteArray data=reply->readAll();
        qDebug()<<"服务器返回数据："<<QString(data);

         //
        QJsonParseError err;
        QJsonDocument rootDoc = QJsonDocument::fromJson(data,&err);
        if(err.error !=QJsonParseError::NoError){

            qDebug()<<"json格式错误";
        }
    else{
                      QJsonObject rootObj =rootDoc.object();
                      QJsonValue codeValue=rootObj.value("code");
    if(codeValue.type()==QJsonValue::String){
         QString code=codeValue.toString();
        qDebug()<<"code:"<<codeValue.toString();
        if(code=="002"){
            QMessageBox::information(this,"提示","成功");
            //跳转
            ui->stackedWidget->setCurrentIndex(0);
        }
        else if(code=="003"){
            QMessageBox::information(this,"提示","用户已存在");
        }
        else if(code=="004"){
            QMessageBox::critical(this,"错误","失败");
            }
        }


    }
        reply->deleteLater();
    });

}



