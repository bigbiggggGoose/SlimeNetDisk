#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include<QPushButton>
#include<QString>
#include<QDebug>
#include<QMouseEvent>
#include<QPropertyAnimation>
#include<QToolButton>
#include <QWidget>
#include <QPainter>

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(QWidget *parent = nullptr);

    // 设置选中状态
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void enterEvent(QEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void minWindow();
    void maxWindow();
    void closeWindow();

    void sigMyFile();
    void sigShare();
    void sigDownload();
    void sigTransform();
    void sigSwitchUser();

private:
    void setHoverStyle(bool hover);  // 设置悬浮样式
    void setPressedStyle(bool pressed); // 设置按压样式

    bool m_hover = false;
    bool m_pressed = false;
    bool m_selected = false;
    QPixmap m_cursorPixmap;
public slots:

};

#endif // MYPUSHBUTTON_H
