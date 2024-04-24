#ifndef CHATSERVER_H 
#define CHATSERVER_H 

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h> 

using namespace muduo ; 
using namespace muduo::net ; 

class ChatServer {
public:
    ChatServer(EventLoop* loop, const InetAddress& listenAddr , const string& nameArg ) ;
    
    void start() ;  



private: 

    // 两个回调函数
    void onConnection(const TcpConnectionPtr&) ; 
    void onMessage(const TcpConnectionPtr& conn , Buffer* buffer, Timestamp receiveTime) ; 
    
    TcpServer _server ; 
    EventLoop* _loop ; 

} ;
 


#endif 