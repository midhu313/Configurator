#ifndef REDISDEFINES_H
#define REDISDEFINES_H

#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>


typedef enum{
    REDIS_CON_OFF,
    REDIS_CON_INIT,
    REDIS_CON_CONNECTED,
    REDIS_CON_DISCONNECTED
}redis_conn_status_t;

typedef struct redis_t{
    redis_conn_status_t status;
    struct event_base *conn_base;
    redisAsyncContext *conn_context;
}redis_t;

#endif // REDISDEFINES_H
