#include "layout.h"
#include"uploadprogress.h"

Layout *Layout::m_instance=new Layout;
Layout::Layout()
{

}
Layout::~Layout()
{

}


Layout *Layout::getInstance(){
    return m_instance;
}

void Layout::setLayout(QWidget *parent){

    QVBoxLayout *vLayout=new QVBoxLayout;
    m_vLayout=vLayout;

    parent->setLayout(m_vLayout);


}

QVBoxLayout * Layout::getLayout(){
        return m_vLayout;
}











