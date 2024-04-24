#include "chatservice.hpp"
#include "public.hpp"

ChatService* ChatService::instance() {
    static ChatService service ; 
    return &service ; 
} 

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
    LOG_INFO << "do register service!!!!" ; 

    
}

MsgHandler ChatService::getMsgHandler(int msgid) {
    if(_msgHandlerMap.find(msgid) == _msgHandlerMap.end() ) {
        return [](const TcpConnectionPtr& , json&  , Timestamp ) -> void {
            LOG_ERROR << "the msgid not have a handler function" ; 
        } ; 
    } 
    return _msgHandlerMap[msgid] ; 
}
