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
*/