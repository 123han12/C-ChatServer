#include "chatserver.hpp"
#include <functional>
#include <iostream>
#include <string>
#include "json.hpp" 
#include "chatservice.hpp"


using json = nlohmann::json ; 
using namespace std;
using namespace placeholders;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));

    _server.setThreadNum(8);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        cout << conn->peerAddress().toIpPort()
             << "   ->   " << conn->localAddress().toIpPort() << "  state:online" << endl;
    }
    else
    {
        cout << conn->peerAddress().toIpPort()
             << "   ->   " << conn->localAddress().toIpPort() << "  state:offline" << endl;

        conn->shutdown(); // 释放相应的套接字资源
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp receiveTime)
{
    string buf = buffer->retrieveAllAsString(); // 从缓冲区中取出来到的消息

    json js = json::parse(buf) ;  // 将数据从字符串转换为相应的类型
    // 完全解耦网络模块的代码和业务模块的代码

    ChatService* service = ChatService::instance() ; 
    MsgHandler handler = service->getMsgHandler(js["msgid"].get<int>() ) ; 
    
    handler(conn , js , receiveTime ) ; 

}