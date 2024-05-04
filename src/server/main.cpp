#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>

void resethandler(int arg){
    ChatService::instance()->reset() ; 
    exit(0) ; 
}

int main(int argc , char** argv ) {

    if(argc < 3 ) {
        std::cout << "please entry  ./a.out  ip  port" << std::endl ; 
        return 0 ; 
    }
    char* ip = argv[1] ; 
    char* port = argv[2] ;
    // 修改默认的处理动作
    signal(SIGINT , resethandler) ; 
    signal(SIGABRT , resethandler) ;     

    EventLoop loop ; 
    InetAddress addr(ip , atoi(port) ) ; 
    ChatServer server(&loop , addr , "ChatServer" ) ; 

    server.start() ;
    loop.loop() ; 
    
    
    return 0 ;
}



/*
{"msgid":2,"name":"masplove","password":"123456"}

{"msgid":1,"id":22,"password":"123456"}

错误测试
{"msgid":1,"id":10086,"password":"123456"}


一对一聊天的消息的格式
"msgid":5
"id":
"name":
"to":
"msg":

测试一对一聊天业务
client1:       {"msgid":1,"id":22,"password":"123456"}     客户1登录
client2:       {"msgid":1,"id":23,"password":"123456"}     客户二登录

client1 send: {"msgid":5,"id":22,"name":"masplove","to":23,"msg":"hello 韩申奥"}   客户1向客户二发送消息 
            {"msgid":5,"id":22,"name":"masplove","to":23,"msg":"hello"}          

client2 send: {"msgid":5,"id":23,"name":"韩申奥","to":22,"msg":"hello,masplove"}   客户二向客户1发送消息


添加好友的业务的json规范  消息类型号为 6  
"msgid":
"id":
"friendid":

    // 例如 22 号向 23 号发起加好友的请求
    {"msgid":6,"id":22,"friendid":23} 


创建群组测试json 
{"msgid":1,"id":22,"password":"123456"}             登录 
{"msgid":7,"id":22,"groupname":"韩申奥","groupdesc":"韩申奥"}             创建群组

这个插入语句有问题，但不清楚什么原因
{"msgid":7,"id":22,"groupname":"高素质人才","groupdesc":"都是高素质人才"}


添加id为23用户到上面的群组(只能自己添加自己)
{"msgid":1,"id":23,"password":"123456"}             23用户登录
{"msgid":8,"id":23,"groupid":2}                    将用户添加到该组中

用户id为22的用户进行一次聊天
{"msgid":9,"id":22,"groupid":2}     



*/