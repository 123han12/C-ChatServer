#include "chatservice.hpp"
#include "redis.hpp"
#include "public.hpp"
#include <vector>
#include <string>

ChatService* ChatService::instance() {
    static ChatService service ; 
    return &service ; 
} 

/*
    为了实现数据库和业务层的分离，这里需要添加一个中间类，从而方便后期的数据库的更换的时候，
    对于业务层的代码不会有一个较大的改动，从而实现了解耦
*/


// 这里需要做一些初始化操作
ChatService::ChatService() {

    // 注册好事件对应的处理函数
    _msgHandlerMap.insert({LOGIN_MSG , bind(&ChatService::login , this , _1 , _2 , _3 ) } ) ; 
    _msgHandlerMap.insert({REG_MSG , bind(&ChatService::Register , this , _1 , _2 , _3 ) } ) ; 
    _msgHandlerMap.insert({ONE_CHAT_MSG , bind(&ChatService::OneChat , this , _1 , _2 , _3 ) } ) ; 
    _msgHandlerMap.insert({ADD_FRIEND_MSG , bind(&ChatService::addFriend , this , _1 , _2 , _3 ) } ) ;
    _msgHandlerMap.insert({CREATE_GROUP_MSG , bind(&ChatService::createGroup , this , _1 , _2 , _3 ) } ) ;
    _msgHandlerMap.insert({ADD_GROUP_MSG , bind(&ChatService::addGroup , this , _1 , _2 , _3 ) } ) ;
    _msgHandlerMap.insert({GROUP_CHAT_MSG , bind(&ChatService::groupChat , this , _1 , _2 , _3 ) } ) ;
    _msgHandlerMap.insert({LOGINOUT_MSG , bind(&ChatService::loginout , this , _1 , _2 , _3 ) } ) ;


        // 连接redis服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }

}


// 处理登录业务
void ChatService::login(const TcpConnectionPtr& conn , json& js , Timestamp time ){
    int id = js["id"] ; 
    std::string pwd = js["password"] ; 
    User user = _userModel.query(id) ; 
    if(user.getId() != -1 && user.getPwd() == pwd ) { // 存在用户信息
        if(user.getState() == std::string( "online") ) {   // 如果此时状态已经是 online 则错误
            json response ; 
            response["msgid"] = LOGIN_MSG_ACk ; 
            response["error"] = -1 ; 
            response["errmsg"] = "user aleary online , please attempt next time" ; 
            conn->send(response.dump()) ; 
        }else {
            {
                lock_guard<mutex> lock(_connMutex) ;  // 这种对象构造的时候加锁，析构的时候解锁 
                _connMap.insert(make_pair(id , conn) ) ;  //在线程互斥的情况下将其进行插入 
            }


            // 登录成功之后，就需要开启响应的订阅
            _redis.subscribe(id) ; 


            user.setState(std::string("online")) ; 
            _userModel.updateState(user) ;  // 更新这个用户的状态 
            json response ; 
            response["msgid"] = LOGIN_MSG_ACk ; 
            response["error"] = 0 ; 
            response["userid"] = user.getId() ; 
            response["username"] = user.getName() ; 

            // 从离线表中取出这个对象的所有离线的消息
            std::vector<std::string> vec = _offlineMessageModel.query(id) ; 
            if(!vec.empty() ) {
                response["offlinemsg"] = vec ; 
                _offlineMessageModel.remove(id) ;  // 移除已经被取出的信息
            }

            // 在登录的时候直接回显好友列表
            std::vector<User> userList = _addFriendModel.query(id) ; 
            if(!userList.empty()){
                std::vector<std::string> strUserList ; 

                for(auto& user: userList ) {
                    json local_js ; 
                    local_js["id"] = user.getId() ; 
                    local_js["name"] = user.getName() ; 
                    local_js["state"] = user.getState() ; 
                    strUserList.push_back(local_js.dump()) ;  
                }
                response["friends"] = strUserList ;  
            }

            // 添加返回用户的群组列表的信息
            std::vector<Group> groupVec = _groupModel.queryGroups(id) ; 
            if(!groupVec.empty()) {
                std::vector<std::string> groupV ; 
                for(Group & group : groupVec) {
                    json groupjs ; 
                    
                    groupjs["id"] = group.getId() ; 
                    groupjs["groupname"] = group.getGroupname() ; 
                    groupjs["groupdesc"] = group.getGroupdesc() ; 
                    std::vector<std::string> userVec ;
                    
                    for(GroupUser& user : group.getUsers() ) {
                        json userjs ; 
                        userjs["id"] = user.getId() ; 
                        userjs["name"] = user.getName() ; 
                        userjs["state"] = user.getState() ;
                        userjs["role"]  = user.getState() ; 
                        userVec.push_back(userjs.dump() ) ; 
                    }
                    groupjs["users"] = userVec ; 
                    groupV.push_back(groupjs.dump() ) ; 
                }
                response["groups"] = groupV ; 
            } 

            conn->send(response.dump()) ; 
        }
    } else {  // 不存在用户信息
        json response ; 
        response["msgid"] = LOGIN_MSG_ACk ; 
        response["error"] = -1 ; 
        response["errmsg"] = "user not exists" ; 
        conn->send(response.dump()) ;
    }
} 


// 处理注册业务
void ChatService::Register(const TcpConnectionPtr& conn , json& js , Timestamp time ) {
    // 实现注册业务
    User user ; 

    std::string name = js["name"] ; 
    std::string password = js["password"] ; 

    user.setName(name) ; 
    user.setPwd(password) ; 

    UserModel model ; 
    bool state  = _userModel.insert(user) ;  
    
    if(state) {    // 注册成功  
        json response ; 
        response["msgid"] = REG_MSG_ACK ; 
        response["error"] = 0 ; 
        response["id"] = user.getId() ; 
        conn->send(response.dump()) ; 
    }else {         // 注册失败
        json response ; 
        response["msgid"] = REG_MSG_ACK ; 
        response["error"] = -1 ; 
        response["errmsg"] = "register error" ; 
        conn->send(response.dump()) ;
    }   

}

MsgHandler ChatService::getMsgHandler(int msgid) {
    if(_msgHandlerMap.find(msgid) == _msgHandlerMap.end() ) {
        return [](const TcpConnectionPtr& , json&  , Timestamp ) -> void {
            LOG_ERROR << "the msgid not have a handler function" ; 
        } ; 
    } 
    return _msgHandlerMap[msgid] ; 
}


void ChatService::clientCloseException(const TcpConnectionPtr& conn ) {
    
    User user ; 
    {
        lock_guard<mutex> lock(_connMutex) ; 
        for(auto iter = _connMap.begin() ; iter != _connMap.end() ; iter ++ ) {
            if(iter->second == conn ){
                user.setId(iter->first) ; 
                _connMap.erase(iter) ; 
                break ;  
            }
        }
    } 
     // 取消异常退出的客户的订阅
    _redis.unsubscribe(user.getId() ) ;  

    // 更新数据库状态
    user.setState(std::string("offline"));
    _userModel.updateState(user) ; 

}


void ChatService::OneChat(const TcpConnectionPtr& conn , json& js , Timestamp time ){

    int toid = js["to"].get<int>(); 
    {
        lock_guard<mutex> lock(_connMutex) ; 
        auto iter = _connMap.find(toid) ; 
        if(iter != _connMap.end() ) {  // 表示此时对端在线，我们就可以进行发送数据了 
            iter->second->send(js.dump() ) ;  // 直接转发
            return  ; 
        }
    }

    // 查询 toid是否在线
    User user = _userModel.query(toid) ; 
    if(user.getState() == "online") {
        _redis.publish(toid , js.dump() ) ; 
    }else {
         // 如果对端处于离线状态，则先将信息存储到离线表中(此时先不考虑集群的情景)
        _offlineMessageModel.insert(toid , js.dump() ) ; 
    }
}


// 将所有人的状态修改为下线
void ChatService::reset() {
    _userModel.resetState() ;
     
}


void ChatService::addFriend(const TcpConnectionPtr& conn , json& js , Timestamp time){
    int userid = js["id"] ; 
    int friendid = js["friendid"] ; 

    // 这里简单起见，直接将好友信息添加到friend 表当中
    _addFriendModel.insert(userid , friendid) ; 

}



// 创建一个群组, 并将该用户以creator 的身份加入到该group当中
void ChatService::createGroup(const TcpConnectionPtr& conn , json& js , Timestamp time) {
    int userid = js["id"] ; 
    std::string name = js["groupname"] ; 
    std::string desc = js["groupdesc"] ;

    Group group(-1 , name , desc ) ; 
    if(_groupModel.createGroup(group ) ) {
        _groupModel.addGroup(userid , group.getId() , "creator") ; 
    }

}

// 将客户端添加到指定的群组
void ChatService::addGroup(const TcpConnectionPtr& conn , json& js , Timestamp time) {

    int userid = js["id"] ; 
    int groupid = js["groupid"] ; 
    _groupModel.addGroup(userid , groupid , "normal") ; 
}

// 进行一次群组内的聊天
void ChatService::groupChat(const TcpConnectionPtr& conn , json& js , Timestamp time) {
    int userid = js["id"].get<int>() ;
    int groupid = js["groupid"].get<int>() ; 
    vector<int> useridVec = _groupModel.queryGroupUsers(userid , groupid ) ; 

    { 
        lock_guard<mutex> lock(_connMutex) ; 
        for(auto userid : useridVec ) {
            auto iter = _connMap.find(userid) ; 
            if(iter != _connMap.end() ) {
                iter->second->send(js.dump() ) ; 
            } else {  // 如果群组中的其他成员此时不在线，则将其存储在离线消息表中

                User user = _userModel.query(userid) ; 
                if(user.getState() == "online" ) {
                    _redis.publish(userid , js.dump() ) ; 
                }else {
                    _offlineMessageModel.insert(userid , js.dump() ) ; 
                } 
            }
        }
    }
    
}


void ChatService::loginout(const TcpConnectionPtr& conn , json& js , Timestamp time){
    int id = js["id"].get<int>() ; // 得到想要退出的用户的id
    
    {
        lock_guard<mutex> lock(_connMutex) ; 
        auto iter = _connMap.find(id) ; 
        if(iter != _connMap.end() ) {
            _connMap.erase(iter) ; 
        }
    }

    // 取消订阅
    _redis.unsubscribe(id) ; 


    User user ; 
    user.setId(id) ; 
    user.setState("offline") ; 
    _userModel.updateState(user);  // 修改其状态为下线

}

// 处理 redis消息队列中发送给在当前服务器登录的客户端的消息
void ChatService::handleRedisSubscribeMessage(int userid , std::string message ) {

    lock_guard<mutex> lock(_connMutex);
    auto it = _connMap.find(userid);
    if (it != _connMap.end() ) 
    {
        it->second->send(message) ;
        return;
    }

    // 存储该用户的离线消息
    _offlineMessageModel.insert(userid , message) ; 
}




/*
需要思考的问题
offlinemesage 如何进行同步， nginx 中的 ip_hash 可以解决这个问题吗？？？？

一个客户端两次登录一定是在同一个服务器上吗？如何解决这个问题,使用ip_hash 一致性哈希算法

多个服务器的 user 表的状态如何进行同步，当前客户端如何获知另一个客户端是否是online状态,这里绝壁有问题, 可以多个集群服务器连接同一个数据库解决这个问题

*/