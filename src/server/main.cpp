#include "chatserver.hpp"
#include <iostream>

int main(int argc , char** argv ) {
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


*/