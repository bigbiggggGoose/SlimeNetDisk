#ifndef TRANSMISSIONWIDGET_H
#define TRANSMISSIONWIDGET_H

#include <QWidget>
#include<QPoint>
#include"common.h"
namespace Ui {
class TransmissionWidget;
}

class TransmissionWidget : public QWidget
{
    Q_OBJECT




public:
    explicit TransmissionWidget(QWidget *parent = nullptr);
    ~TransmissionWidget();
    void showPage(TransmitStatus status);
private:
    Ui::TransmissionWidget *ui;

    void dispayDataRecord(QString path=RECORD_DIR);
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_clear_clicked();
};

#endif // TRANSMISSIONWIDGET_H
