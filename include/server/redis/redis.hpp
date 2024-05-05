#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
#include <string> 

class Redis {

public:
    Redis() ; 
    ~Redis() ;

    // 连接 redis 服务器
    bool connect() ; 

    // 向redis 指定的通道channel 发布消息
    bool publish(int channel , std::string message ) ; 

    // 向redis指定的通道subscribe 订阅消息
    bool subscribe(int channel ) ; 

    // 向redis 指定通道unsubscribe 取消订阅
    bool unsubscribe(int channel) ; 

    // 在独立线程中接受订阅通道中的消息
    void observer_channel_message() ; 

    // 初始化向业务层上报通道消息的回调对象
    void init_notify_handler(std::function<void(int , std::string)> fn ) ; 

private:
    // hiredis同步上下文对象，负责pushlish 消息
    redisContext* _publish_context  ; 

    // hiredis 同步对象，负责subscribe 消息
    redisContext* _subscribe_context ; 

    std::function<void(int , std::string) > _notify_message_handler ; 

} ; 


#endif 