#include "titlewidget.h"
#include "ui_titlewidget.h"
#include<QMouseEvent>
#include<QPainter>

TitleWidget::TitleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleWidget)
{
    ui->setupUi(this);



    //去掉标题栏
    this->setWindowFlags(Qt::FramelessWindowHint|windowFlags());


   // 让 widget 在没有按下鼠标按钮的情况下 也能接收 mouseMoveEvent 鼠标移动事件
   // 鼠标悬停高亮按钮 ✅ 需要
   // 自定义悬浮提示 ✅ 需要
   setMouseTracking(true);


}

TitleWidget::~TitleWidget()
{
    delete ui;
}

void TitleWidget::mousePressEvent(QMouseEvent *event){

    if(event->buttons()==Qt::LeftButton){
        cha=event->globalPos() - this->window()->geometry().topLeft();
    }
}

void TitleWidget::mouseMoveEvent(QMouseEvent *event){
    QWidget *topLevel = this->window();
    if(event->buttons()&Qt::LeftButton){
         topLevel->move(event->globalPos()-cha);
    }
}

void TitleWidget::paintEvent(QPaintEvent *event){
    QPainter p(this);

    QPixmap bg(":/Resource/bgp3.jpg");
    p.drawPixmap(0,0,this->width(),this->height(),bg);

}


