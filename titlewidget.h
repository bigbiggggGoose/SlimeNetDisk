#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include<QPoint>

namespace Ui {
class TitleWidget;
}

class TitleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TitleWidget(QWidget *parent = nullptr);
    ~TitleWidget();
protected:
      void mouseMoveEvent(QMouseEvent *event);
      void mousePressEvent(QMouseEvent *event);
      void paintEvent(QPaintEvent *event);
private:
    Ui::TitleWidget *ui;
    QPoint cha;

};

#endif // TITLEWIDGET_H
