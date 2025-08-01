#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QToolButton>
#include<QSignalMapper>
#include<QMap>
#include"mypushbutton.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUser(QString user);
private:
    Ui::MainWindow *ui;
    QPoint cha;
    MyPushButton* m_currentSelectedButton = nullptr;



    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private slots:
    void onButtonClicked();
    void Signals();

signals:
    void sigChangeuser();
    void sigLoginAgain();
};
#endif // MAINWINDOW_H
