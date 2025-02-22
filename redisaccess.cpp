#include "redisaccess.h"

RedisAccess::RedisAccess(QObject *parent) : QObject(parent), timeout({0,1000})
{
   isConnected = false;

}

RedisAccess::~RedisAccess(){

}

void RedisAccess::connect(QString ip){
    ip_address = ip;
    m_RedisContext = redisConnectWithTimeout(ip_address.toUtf8().constData(),6379,timeout);
    if(m_RedisContext == NULL || m_RedisContext->err){
        emit redisErrorMessage("Unable to connect!");
        isConnected=false;
    }else
        isConnected=true;
    emit connectionStatusChange(isConnected);
}

void RedisAccess::disconnect(){
    if(isConnected)
        redisFree(m_RedisContext);
    isConnected = false;
    emit connectionStatusChange(isConnected);
}

void RedisAccess::reconnect(){
    if(isConnected){
        redisFree(m_RedisContext);
        m_RedisContext = redisConnectWithTimeout(ip_address.toUtf8().constData(),6379,timeout);
        if(m_RedisContext == NULL || m_RedisContext->err){
            emit redisErrorMessage("Unable to reconnect!");
            isConnected=false;
        }else
            isConnected=true;
        emit connectionStatusChange(isConnected);
    }
}

int RedisAccess::publish(QString channel,QString payload){
    int rets =0,repeat=1;
    if(!isConnected)
        reconnect();
    if(isConnected){
        do{
            m_RedisReply = (redisReply*)redisCommand(m_RedisContext,"PUBLISH %s %s",
                channel.toUtf8().constData(),payload.toUtf8().constData());
            if(m_RedisReply){
                rets = m_RedisReply->integer;
                freeReplyObject(m_RedisReply);
                break;
            }else{
                reconnect();
            }
        }while(repeat--);
    }
    return rets;
}

int RedisAccess::publish(QString channel,void *data,size_t len){
    int rets =0,repeat=1;
    if(!isConnected)
        reconnect();
    if(isConnected){
        do{
            m_RedisReply = (redisReply*)redisCommand(m_RedisContext,"PUBLISH %s %b",
                channel.toUtf8().constData(),data,len);
            if(m_RedisReply){
                rets = m_RedisReply->integer;
                freeReplyObject(m_RedisReply);
                break;
            }else{
                reconnect();
            }
        }while(repeat--);
    }
    return rets;
}
