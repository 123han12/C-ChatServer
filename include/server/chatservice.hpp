#ifndef CHATSERVICE_H 
#define CHATSERVICE_H

#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include "json.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <usermodel.hpp>
#include <memory>

using namespace std ; 
using namespace muduo ; 
using namespace muduo::net ; 
using namespace placeholders ; 
using json = nlohmann::json ; 



// 定义函数对象
using MsgHandler = std::function<void(const TcpConnectionPtr &conn , json& js , Timestamp ) > ;  

// 给 msgid 映射一个事件处理函数
// 单例模式设计聊天的服务类
class ChatService {


public:

    // 获取单例对象的接口函数
    static ChatService* instance() ; 

    // 处理登录业务
    void login(const TcpConnectionPtr& conn , json& js , Timestamp time ) ; 
    
    // 处理注册业务
    void Register(const TcpConnectionPtr& conn , json& js , Timestamp time ) ; 
    
    MsgHandler getMsgHandler(int msgid) ; 

private:

    ChatService() ; 
    
    
    // 存储消息id和其对应的业务处理方法
    unordered_map<int , MsgHandler> _msgHandlerMap ; //消息id对应的业务处理函数


} ; 


#endif 
