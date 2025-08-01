#ifndef LOGININFOINSTANCE_H
#define LOGININFOINSTANCE_H

#include<QString>

class LoginInfoInstance
{
public:
   static  LoginInfoInstance* getInstance();

   void setUser(QString user);
   void setToken(QString token);
   void setIp(QString ip);
   void setPort(QString port);
   QString getUser();
   QString getToken();
   QString getIp();
   QString getPort();

private:
     LoginInfoInstance();
     static LoginInfoInstance* m_instance;

    QString user;
    QString token;
    QString ip;
    QString port;

};

#endif // LOGININFOINSTANCE_H
