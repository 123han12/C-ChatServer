#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>

void resethandler(int arg){
    ChatService::instance()->reset() ; 
    exit(0) ; 
}

int main(int argc , char** argv ) {

    // 修改默认的处理动作
    signal(SIGINT , resethandler) ; 
    signal(SIGABRT , resethandler) ;     

    EventLoop loop ; 
    InetAddress addr("127.0.0.1" , 9527) ; 
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


*/