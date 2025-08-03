 #ifndef MYMENU_H
#define MYMENU_H

#include <QMenu>

class MyMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MyMenu(QWidget *parent = nullptr); //父类 QMenu继承QWidget

signals:

};

#endif // MYMENU_H
