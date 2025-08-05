#include "transmissionwidget.h"
#include "ui_transmissionwidget.h"
#include<QMouseEvent>
#include<QPainter>
#include"layout.h"
#include"LoginInfoInstance.h"
TransmissionWidget::TransmissionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransmissionWidget)
{
    ui->setupUi(this);

    //垂直布局
    Layout *layout =Layout::getInstance();
    layout->setLayout(ui->tab_1);

    //传输记录
    connect(ui->tabWidget,&QTabWidget::currentChanged,this,[=](int index){
        if(!index){

        }else if(index==1){
            dispayDataRecord();
        }
    });


}

TransmissionWidget::~TransmissionWidget()
{
    delete ui;
}

void TransmissionWidget::dispayDataRecord(QString path){
    QString recordDir = path;
    QString recordFilePath = recordDir + "/" + LoginInfoInstance::getInstance()->getUser() + ".txt";


    QByteArray old;
    QFile file(recordFilePath);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "无法以只读模式打开日志文件:" << recordFilePath;
            return;
        }
        old = file.readAll();
        file.close();
    }

    ui->recordEdit->setText(QString::fromLocal8Bit(old));

    if(file.isOpen())file.close();

}

void TransmissionWidget::paintEvent(QPaintEvent *event){
    QPainter p(this);

    QPixmap bg(":/Resource/bgp3.jpg");
    p.drawPixmap(0,0,this->width(),this->height(),bg);

}

void TransmissionWidget::showPage(TransmitStatus status){
    if(status==TransmitStatus::Record){
        ui->tabWidget->setCurrentWidget(ui->tab_2);
    }else{
        ui->tabWidget->setCurrentWidget(ui->tab_1);
    }
}






void TransmissionWidget::on_clear_clicked()
{
    ui->recordEdit->setText("");
}
