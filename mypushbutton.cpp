#include "mypushbutton.h"


MyPushButton::MyPushButton(QWidget *parent)
    : QPushButton(parent)  // 初始化父类构造
{



}

void MyPushButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    // 如果按钮是选中状态，绘制光标指示器
    if (m_selected) {
        QPainter painter(this);
        QRect cursorRect(width() - m_cursorPixmap.width() - 5,
                        (height() - m_cursorPixmap.height()) / 2,
                        m_cursorPixmap.width(),
                        m_cursorPixmap.height());
        painter.drawPixmap(cursorRect, m_cursorPixmap);
    }
}
void MyPushButton::setSelected(bool selected)
{
    m_selected = selected;
    update();
}

// 鼠标进入事件
void MyPushButton::enterEvent(QEvent *e) {
    setHoverStyle(true);
    QPushButton::enterEvent(e);
}
// 鼠标离开事件
void MyPushButton::leaveEvent(QEvent *e) {
    setHoverStyle(false);
    QPushButton::leaveEvent(e);
}

void MyPushButton::mousePressEvent(QMouseEvent *e)
{

    if (e->button() == Qt::LeftButton) {
            setPressedStyle(true);
        }
        QPushButton::mousePressEvent(e);


    QString btnName = this->objectName(); // 通过 objectName 判断用途 
    if (btnName == "minimize") {
        emit minWindow();
    }
    else if (btnName == "maximize") {
        emit maxWindow();
    }
    else if (btnName == "close_button") {
        emit closeWindow();
    }

    else if (btnName == "MyFile") {
        emit sigMyFile();
    }
    else if (btnName == "Share") {
        emit sigShare();
    }
    else if (btnName == "Download") {
        emit sigDownload();
    }
    else if (btnName == "TransformationList") {
        emit sigTransform();
    }
    else if (btnName == "SwitchUser") {
        emit sigSwitchUser();
    }

}


// 鼠标释放事件
void MyPushButton::mouseReleaseEvent(QMouseEvent *e) {
    setPressedStyle(false);
    QPushButton::mouseReleaseEvent(e);
}

// 设置悬浮样式
void MyPushButton::setHoverStyle(bool hover) {
    if (hover) {
        setStyleSheet(R"(
            QPushButton {
                background: white;
            }
        )");
    } else {
        setStyleSheet(R"(
            QPushButton {
                background: rgb(225,225,225);
            }
        )" );
    }
}

// 设置按压样式
void MyPushButton::setPressedStyle(bool pressed) {
    if (pressed) {

    } else {

        setHoverStyle(underMouse()); // 恢复悬浮状态
    }
}


