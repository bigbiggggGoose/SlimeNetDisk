#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"mypushbutton.h"
#include<QMouseEvent>
#include <QPushButton>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->myfile_page);



    setMouseTracking(true);
    this->setWindowFlags(Qt::FramelessWindowHint|windowFlags());


    ui->myfile_page->setStyleSheet("background-color: rgb(200,250,250);");
    ui->share_page->setStyleSheet("background-color: rgb(250,232,240);");
    ui->download_page->setStyleSheet("background-color: rgb(229,220,255);");
    ui->transform_page->setStyleSheet("background-color: rgb(255,200,200);");
    Signals();
    //设置默认：
    m_currentSelectedButton = ui->MyFile;
    ui->MyFile->setSelected(true);
    // 连接按钮点击信号
    connect(ui->MyFile, &MyPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(ui->Share, &MyPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(ui->Download, &MyPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(ui->TransformationList, &MyPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(ui->SwitchUser, &MyPushButton::clicked, this, &MainWindow::onButtonClicked);

    connect(ui->myfile_page,&MyFileWidget::gotoTransmission,this,[=](TransmitStatus status){
        ui->TransformationList->setChecked(true);
        ui->stackedWidget->setCurrentWidget(ui->transform_page);
        ui->transform_page->showPage(status);
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Signals(){
    // 连接信号槽
    connect(ui->minimize, &MyPushButton::minWindow, this, &MainWindow::showMinimized);
    connect(ui->maximize, &MyPushButton::maxWindow, this, [=](){
            if (isMaximized())
                this->showNormal();
            else
                this->showMaximized();
        });
    connect(ui->close_button, &MyPushButton::closeWindow, this, &MainWindow::close);

    connect(ui->MyFile, &MyPushButton::sigMyFile, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->myfile_page);
    });
    connect(ui->Share, &MyPushButton::sigShare, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->share_page);

    });
    connect(ui->Download, &MyPushButton::sigDownload, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->download_page);
    });
    connect(ui->TransformationList, &MyPushButton::sigTransform, this, [=](){
        ui->stackedWidget->setCurrentWidget(ui->transform_page);
    });
    connect(ui->SwitchUser, &MyPushButton::sigSwitchUser, this, [=](){
      emit sigChangeuser();
    });

    //token失效时
    connect(ui->myfile_page, &MyFileWidget::sigLoginAgain, this, [=](){
      emit sigLoginAgain();
    });

}

void MainWindow::onButtonClicked(){
   MyPushButton* clickedButton = qobject_cast<MyPushButton*>(sender());
    if (!clickedButton || clickedButton == m_currentSelectedButton) {
        return;
    }
 // 取消之前选中按钮的状态
    if (m_currentSelectedButton) {
        m_currentSelectedButton->setSelected(false);
    }

    // 设置新选中按钮
    clickedButton->setSelected(true);
    m_currentSelectedButton = clickedButton;

}




void MainWindow::mousePressEvent(QMouseEvent *event){

    if(event->buttons()==Qt::LeftButton){
        cha=event->globalPos() - this->window()->geometry().topLeft();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    QWidget *topLevel = this->window();
    if(event->buttons()&Qt::LeftButton){
         topLevel->move(event->globalPos()-cha);
    }
}

void MainWindow::setUser(QString user){
    ui->User->setText(user);
}
