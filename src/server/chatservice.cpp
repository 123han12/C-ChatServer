#include "chatservice.hpp"
#include "public.hpp"

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

}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr& conn , json& js , Timestamp time ){
    LOG_INFO << "do login service!!!!" ; 


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
    bool state  = model.insert(user) ; 
    
    if(state ) {    // 注册成功  
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
