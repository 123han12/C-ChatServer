#ifndef PUBLIC_H 
#define PUBLIC_H 

/*
    server 和 client 的公共文件
*/

enum EnMsgType {
    LOGIN_MSG = 1 , 
    REG_MSG , 
    REG_MSG_ACK ,  // 注册成功的响应消息
    LOGIN_MSG_ACk, 
    ONE_CHAT_MSG ,   // 聊天的消息类型
} ; 


#endif 
