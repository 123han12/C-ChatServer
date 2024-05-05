#ifndef CHATSERVICE_H 
#define CHATSERVICE_H

#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include "json.hpp"
#include "public.hpp"
#include "offlinemessagemodel.hpp"
#include "addfriendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp" 


#include <muduo/base/Logging.h>
#include <usermodel.hpp>
#include <memory>
#include <mutex>

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
    
    // 获取对应消息的句柄
    MsgHandler getMsgHandler(int msgid) ; 

    void OneChat(const TcpConnectionPtr& conn , json& js , Timestamp time ) ; 

    void clientCloseException(const TcpConnectionPtr& conn ) ; 


    void reset() ;  // 将所有的人的状态都修改为下线。

    void addFriend(const TcpConnectionPtr& conn , json& js , Timestamp time) ; 

    void createGroup(const TcpConnectionPtr& conn , json& js , Timestamp time) ; 

    void addGroup(const TcpConnectionPtr& conn , json& js , Timestamp time) ; 

    void groupChat(const TcpConnectionPtr& conn , json& js , Timestamp time)  ; 

    void loginout(const TcpConnectionPtr& conn , json& js , Timestamp time) ; 

    void handleRedisSubscribeMessage(int channel  , std::string message ) ; 

private:

    ChatService() ; 
    
    
    // 存储消息id和其对应的业务处理方法
    unordered_map<int , MsgHandler> _msgHandlerMap ; 

    // 用于实现集群聊天服务器的互通有无
    unordered_map<int, TcpConnectionPtr > _connMap ; // tcpid -> TcpConnectionPtr ，

    mutex  _connMutex ;      


    UserModel _userModel ; 
    OfflineMessageModel _offlineMessageModel ; 
    addFriendModel _addFriendModel ; 
    GroupModel _groupModel ; 

    Redis _redis ; 

} ; 


#endif 
