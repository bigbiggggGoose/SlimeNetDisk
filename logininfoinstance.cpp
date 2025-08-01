#include "logininfoinstance.h"
LoginInfoInstance* LoginInfoInstance::m_instance= new LoginInfoInstance;
LoginInfoInstance::LoginInfoInstance()
{

}

LoginInfoInstance*  LoginInfoInstance:: getInstance(){
    if (m_instance == nullptr) {
            m_instance = new LoginInfoInstance();
      }
    return m_instance;
}

void LoginInfoInstance::setUser(QString user){
    this->user=user;
}
void LoginInfoInstance::setToken(QString token){
    this->token=token;
}
void LoginInfoInstance::setIp(QString ip){
    this->ip=ip;
}
void LoginInfoInstance::setPort(QString port){
    this->port=port;
}
QString LoginInfoInstance::getUser(){
    return user;
}

QString LoginInfoInstance::getToken(){
    return token;
}
QString LoginInfoInstance::getIp(){
    return ip;
}
QString LoginInfoInstance::getPort(){
    return port;
}
