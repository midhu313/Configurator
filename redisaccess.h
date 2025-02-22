#ifndef REDISACCESS_H
#define REDISACCESS_H

#include <QObject>
#include <QDebug>
#include <sys/time.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>

//struct Config_Params_S{
//    char app_name[40];
//    char
//};


class RedisAccess : public QObject
{
    Q_OBJECT
public:
    explicit RedisAccess(QObject *parent = nullptr);
    ~RedisAccess();

    void connect(QString ip);
    void disconnect();

    int publish(QString channel,QString payload);
    int publish(QString channel,void *data,size_t len);
signals:
    void connectionStatusChange(bool state);
    void redisErrorMessage(QString msg);

private:
    QString ip_address;
    struct timeval timeout;

    redisContext *m_RedisContext;
    redisReply *m_RedisReply;

    bool isConnected;
    void reconnect();

};

#endif // REDISACCESS_H
