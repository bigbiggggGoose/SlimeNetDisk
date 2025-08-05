#ifndef LAYOUT_H
#define LAYOUT_H
#include<QWidget>
#include<QVBoxLayout>
class Layout
{
public:
    static Layout *getInstance();
    void setLayout(QWidget *parent);
    QVBoxLayout *getLayout();

private:
    Layout();
    ~Layout();

    static Layout *m_instance;

    QVBoxLayout *m_vLayout;


};

#endif // LAYOUT_H
