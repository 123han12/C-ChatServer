/*
此文件为客户端程序进行开发
*/
#include "json.hpp"
#include "group.hpp"
#include "groupuser.hpp"
#include "public.hpp"

#include <vector>
#include <thread>
#include <string>
#include <unordered_map>
#include <functional>
#include <ctime>

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

User globalCurrUser ; // 记录在当前客户端登录的用户的信息
std::vector<Group> globalCurrUserGroups; // 记录群聊组
std::vector<User>  globalCurrUserFriends; // 当前用户的好友列表

bool LOGINOUT  = true ;  // 为true 表示此时没有用户登录，为false 表示此时有用户登录  

// 展示主菜单
void showmenu() {
    std::cout << "===============================" << std::endl ;     
    std::cout << "1 : register" << std::endl ; 
    std::cout << "2 : login" << std::endl ; 
    std::cout << "3 : exit" << std::endl ; 
    std::cout << "===============================" << std::endl ;     
}

// 以字符串的形式获取当前时间
std::string getCurrentTime() {
    time_t now = time(0) ; 
    char* dt = ctime(&now) ; 
    int len = strlen(dt) ; 
    return std::string(dt).substr(0 , len - 1 ) ;  
}

//展示当前用户的好友信息和群组信息
void showCurrUserData() {
    std::cout << "===========================================================" << std::endl ;     
    // 展示好友
    for(auto& user : globalCurrUserFriends ) {
        std::cout << "id: " << user.getId() << "     name: "  << user.getName() << "     state: " <<  user.getState() << std::endl ;  
    }
    std::cout << "===========================================================" << std::endl ;     
    for(auto& group : globalCurrUserGroups ) {
        std::cout << "----------------------------------------------" << std::endl ; 
        std::cout << "group id: " << group.getId() << "      group name: " << group.getGroupname() << "      group descripton: " << group.getGroupdesc() << std::endl ; 
        std::cout << "User of this group:" << std::endl ; 
        for(auto& user : group.getUsers() ) {
            std::cout << "          " << "id: " << user.getId() << "    name: "  << user.getName() << "    state: " <<  user.getState() << std::endl ;
        }
    }
    std::cout << "===========================================================" << std::endl ;     
}

// 用于接受远端的数据，读
void readTaskHandler(int clientfd ) {
    for(; ; ) {
        char buffer[1024] = {'\0'} ;

        int len = recv(clientfd , buffer , 1024 , 0 ) ;  
        if(len == -1 || len == 0 ) {
            close(clientfd) ; 
            std::cout << "readTaskHnadler  recv function error" << std::endl ; 
            exit(-1) ; 
        }
        nlohmann::json js = nlohmann::json::parse(buffer) ; 
        if(ONE_CHAT_MSG == js["msgid"].get<int>() ) {
            std::cout << js["time"].get<std::string>() << "\n[" << js["id"] << "]" << 
            js["name"].get<std::string>() << " said: " << js["msg"] << std::endl ; 
        }else if(GROUP_CHAT_MSG == js["msgid"].get<int>() ) { // 如果是一个群组消息的话
            std::cout << js["time"].get<std::string>() << "\ngroup[" << js["groupid"] << "]message" << "[" << js["id"] << "]" << 
            js["name"].get<std::string>() << " said: " << js["msg"] << std::endl ; 
        }
    }
}



// 一系列的命令处理函数
void help(int a = 0 , std::string = "" ) ; 
void chat(int , std::string) ;
void addfriend(int , std::string) ;
void creategroup(int , std::string) ;
void addgroup(int , std::string) ;
void groupchat(int , std::string) ;
void loginout(int , std::string) ;

// 注册系统支持的客户端命令处理
std::unordered_map<std::string , std::function<void(int , std::string) >> commandHandlerMap = {
    {"help" , help } ,
    {"chat" , chat } , 
    {"addfriend" , addfriend} , 
    {"creategroup" , creategroup } , 
    {"addgroup" , addgroup } , 
    {"groupchat" , groupchat } , 
    {"loginout" , loginout } 
}  ; 

// 注册每个api的使用格式
std::unordered_map<std::string , std::string> commandMap = {
    {"help" , "显示所有支持的命令,格式help"} , 
    {"chat" , "一对一聊天, 格式chat:friendid:message"} , 
    {"addfriend" , "添加好友,格式addfriend:friendid"} , 
    {"creategroup" , "创建群组,格式creategroup:groupname:groupdesc"} , 
    {"addgroup" , "加入群组,格式addgroup:groupid"} , 
    {"groupchat" , "群聊, 格式groupchat:groupid:message"} , 
    {"loginout" , "注销,格式loginout"}
} ;

// 用于向远端发送数据，写
void mainMenu(int clientfd ) {
    help() ; 

    while(!LOGINOUT) { // 如果没有退出
        char comm[50] = {'\0' } ; 
        std::cin.getline(comm , 50 ) ; 
        std::string commandStr = comm ; 
        int idx = commandStr.find(':') ; 

        std::string command ;  
        if(idx == std::string::npos ) {  // 没有找到    
            command = commandStr ; 
        }else {
            command = commandStr.substr(0 , idx ) ;
        }

        if(commandHandlerMap.find(command) != commandHandlerMap.end() ) {
            std::string Args = commandStr.substr(idx + 1 , commandStr.size() - idx ) ; 
            std::function<void(int , std::string)> Handler = commandHandlerMap.find(command)->second ; 
            Handler(clientfd , Args ) ;  
        }else {
            std::cerr << "command not found please try again" << std::endl ;   
        }
    }

}

int main(int argc , char** argv ) {
    if(argc < 3 ) {
        std::cout << "please entry  ./a.out  ip  port" << std::endl ; 
        return 0 ; 
    }
    char* ip = argv[1] ; 
    char* port = argv[2] ; 
    
    int clientfd = socket(AF_INET , SOCK_STREAM , 0 ) ; 

    sockaddr_in addr ;
    addr.sin_family = AF_INET ; 
    addr.sin_port = ntohs(atoi(port) ) ; 
    inet_pton(AF_INET , ip , &addr.sin_addr.s_addr ); 

    if(connect(clientfd , (sockaddr* )&addr , sizeof(addr)) == -1 ) {

        std::cout << "connection failed!" << std::endl ; 
        exit(-1) ;
    } 
    std::cout << "connection sucess!" << std::endl ; 

    for(; ; ){
        showmenu() ; 

        std::cout << "you choice:" ;
        int choice = 0 ; 
        std::cin >> choice ; 
        std::cin.get() ;  // 读出多余的空格
        switch(choice) {
            case 1:   // 处理注册业务
                {
                    char name[50] = {0} ; 
                    char password[50] = {0} ;
                    std::cout << "username:";
                    std::cin.getline(name , 50) ; // 读取一行数据到name 
                    std::cout << "password:" ; 
                    std::cin.getline(password , 50) ; 
                    
                    nlohmann::json js ; 
                    js["name"] = std::string(name) ; 
                    js["password"] = std::string(password) ; 
                    js["msgid"] = REG_MSG ;    
                    std::string request = js.dump() ; 

                    int ret = send(clientfd , request.c_str() , strlen(request.c_str()) + 1 , 0 ) ; 
                    if(ret == -1 ) { // 发送失败
                        std::cerr << "send register message error" << std::endl ; 
                    } else {    // 发送成功 , 接受服务端发送的信息
                        char buffer[1024] = {0} ; 
                        int len = recv(clientfd , buffer , 1024 , 0 ) ; 
                        if(len == -1 ) { // 读取失败
                            std::cerr << "recv response msg error" << std::endl ; 
                        } else { // 读取成功
                            std::cout << "recv response sucess!" << std::endl ; 
                            nlohmann::json response = nlohmann::json::parse(buffer) ;  // 反序列话数据
                            std::cout << "parse response sucess!" << std::endl ;
                            int err = response["error"].get<int>() ; 
                            if(err == -1 ) { // 注册失败
                                std::cerr << response["errmsg"] << std::endl ;  
                            }else { // 注册成功
                                std::cout << "register sucess you id is:" << response["id"].get<int>() << std::endl ; 
                            }
                        }
                    }
                    break ; 
                } 
            case 2:  // 处理登录业务 , 一旦进行登录，这里就需要进行一些多线程的处理
                {
                    int id = -1 ; 
                    char password[50] = {'\0'} ; 
                    std::cout << "id:";
                    std::cin >> id ; 
                    std::cin.get() ; // 读取空格
                    std::cout << "password:" ; 
                    std::cin.getline(password , 50) ; // 读取到密码
                    nlohmann::json js ; 
                    js["msgid"] = LOGIN_MSG ; 
                    js["id"] = id ; 
                    js["password"] = password ; 

                    std::string request = js.dump() ; // 序列化为字符串
                    
                    int ret = send(clientfd , request.c_str() , strlen(request.c_str()) + 1 , 0 ) ; 
                    if(ret == -1 ){ // 这里表示发送失败
                        std::cerr << "send login message error" << std::endl ; 
                    }else { //这里表示发送成功，需要接受数据
                        char buffer[BUFSIZ] = {'\0'}; 
                        
                        int len = recv(clientfd , buffer , BUFSIZ , 0 ) ; 
                        if(len == -1 ) {
                            std::cerr << "recv login message error " << std::endl ;
                        }else {
                            std::cerr << "recv login message sucess!" << std::endl ; 
                            
                            nlohmann::json response = nlohmann::json::parse(buffer) ; // 解析出json对象
                            int error = response["error"].get<int>() ; 
                            if(error == -1 ) { // 登录失败。
                                std::cerr << response["errmsg"] << std::endl ; 
                            } else if(error == 0 ) { // 登录成功
                                // 设置全局变量
                                globalCurrUser.setId(response["userid"].get<int>() ) ; 
                                globalCurrUser.setName(response["username"].get<std::string>() ) ; 
                                
                                std::cout << globalCurrUser.getName() << "  login sucess" << std::endl ; 
                                
                                // 记录好友列表信息
                                if(response.contains("friends") ) {
                                    globalCurrUserFriends.clear() ; 
                                    std::vector<std::string> userList = response["friends"] ; 

                                    for(auto& strjs : userList ) {
                                        nlohmann::json userjs = nlohmann::json::parse(strjs) ; 
                                        User friends ;
                                        friends.setId(userjs["id"].get<int>() ) ; 
                                        friends.setName(userjs["name"].get<std::string>() ) ; 
                                        friends.setState(userjs["state"].get<std::string>()) ;   
                                        globalCurrUserFriends.push_back(friends) ; 
                                    }

                                } 
                                // 记录当前用户的群组信息
                                if(response.contains("groups")) {
                                    globalCurrUserGroups.clear() ;
                                    std::vector<std::string> groupV = response["groups"] ; 
                                    for(auto& groupstr : groupV ) {
                                        nlohmann::json groupjs = nlohmann::json::parse(groupstr) ; 
                                        Group group ; 
                                        group.setId(groupjs["id"].get<int>() ) ; 
                                        group.setGroupname(groupjs["groupname"]) ; 
                                        group.setGroupDesc(groupjs["groupdesc"]) ; 

                                        std::vector<std::string> userStrList = groupjs["users"] ; 
                                        for(auto& userStr : userStrList ) {
                                            nlohmann::json userjs = nlohmann::json::parse(userStr) ; 
                                            GroupUser user ; 
                                            user.setId(userjs["id"].get<int>() ) ; 
                                            user.setName(userjs["name"] ) ; 
                                            user.setState(userjs["state"] );
                                            user.setRole(userjs["role"] ) ; 

                                            group.getUsers().push_back(user) ; 
                                        }

                                        globalCurrUserGroups.push_back(group) ; 
                                    }
                                }
                                showCurrUserData() ; 
                                
                                // 显示当前用户的离线消息
                                if(response.contains("offlinemsg")) {
                                    std::vector<std::string> offlinemsgvec = response["offlinemsg"] ; 
                                    for(auto& offlinemsg : offlinemsgvec ) {

                                        nlohmann::json js = nlohmann::json::parse(offlinemsg) ; 
                                        
                                        if(ONE_CHAT_MSG == js["msgid"].get<int>() ) {
                                       
                                            std::cout << js["time"].get<std::string>() << "[" << js["id"] << "]" << 
                                            js["name"].get<std::string>() << " said: " << js["msg"] << std::endl ; 
                                       
                                        }else if(GROUP_CHAT_MSG == js["msgid"].get<int>() ) { // 如果是一个群组消息的话
                                       
                                            std::cout << js["time"].get<std::string>() << "group[" << js["groupid"] << "]message" << "[" << js["id"] << "]" << 
                                            js["name"].get<std::string>() << " said: " << js["msg"] << std::endl ; 
                                       
                                        }

                                    }
                                }
                                LOGINOUT = false ;  
                                static int control = 0 ;  // 控制该程序一定只会创建一个新的线程
                                if(control == 0 ) {
                                    std::thread readTask(readTaskHandler , clientfd ) ; 
                                    readTask.detach() ;
                                    control ++ ;  
                                }
                                mainMenu(clientfd) ; // 进入主聊天页面，主要用来发送数据
                            } 
                        }
                    }

                }
                break ; 
            case 3:
                exit(0) ; 
            default:
                std::cout << "input error please try again" << std::endl ; 
                break ; 
        }

    }

    return 0 ; 
}


void help(int , std::string) {

    // 输出每一个命令的使用格式
    for(auto& str: commandMap ) {
        std::cout << str.first << "  "<< str.second << std::endl ; 
    }
}

/*1.拆分参数，2.构建json字符串，3.发送*/

// {"chat" , "一对一聊天, 格式chat:friendid:message"} 
void chat(int fd , std::string commandMessage ) {
    // 拆分参数
    int idx = commandMessage.find(':') ; 
    if(idx == std::string::npos ) {
        std::cout << "args error" << std::endl ; 
        return ; 
    } 
    int friendid = atoi(commandMessage.substr(0 , idx ).c_str() ) ; 
    std::string message = commandMessage.substr(idx + 1) ; 

    // 构建json字符串
    nlohmann::json request ; 
    request["msgid"] =  ONE_CHAT_MSG ; 
    request["to"] = friendid ; 
    request["id"] = globalCurrUser.getId() ; 
    request["name"] = globalCurrUser.getName() ; 
    request["msg"] = message ; 
    request["time"] = getCurrentTime() ; 

    std::string requestStr = request.dump() ;  

    // 发送数据
    int len = send(fd , requestStr.c_str() , strlen(requestStr.c_str() ) + 1 , 0 ) ; 
    if(len == -1) {
        std::cout << "send error" << std::endl ;
        return ;  
    } 

}

// {"addfriend" , "添加好友,格式addfriend:friendid"} , 
void addfriend(int fd , std::string commandMessage) {
    
    // 拆分参数 
    int friendid = atoi(commandMessage.c_str() ) ;  

    // 构建json字符串
    nlohmann::json request ; 
    request["msgid"] =  ADD_FRIEND_MSG ; 
    request["id"] = globalCurrUser.getId() ; 
    request["friendid"] = friendid ; 
    std::string requestStr = request.dump() ;  

    // 发送数据
    int len = send(fd , requestStr.c_str() , strlen(requestStr.c_str() ) + 1 , 0 ) ; 
    if(len == -1) {
        std::cout << "send error" << std::endl ;
        return ;  
    } 
}

// {"creategroup" , "创建群组,格式creategroup:groupname:groupdesc"} 
void creategroup(int fd  , std::string commandMessage) {
    // 拆分参数
    int idx = commandMessage.find(':') ; 
    if(idx == std::string::npos ) {
        std::cout << "args error" << std::endl ; 
        return ; 
    } 
    std::string groupname = commandMessage.substr(0 , idx ) ;  
    std::string groupdesc = commandMessage.substr(idx + 1) ; 

    // 构建json字符串
    nlohmann::json request ; 
    request["msgid"] =  CREATE_GROUP_MSG ; 
    request["id"] = globalCurrUser.getId() ; 
    request["groupname"] = groupname ;  
    request["groupdesc"] = groupdesc ; 

    std::string requestStr = request.dump() ;  

    // 发送数据
    int len = send(fd , requestStr.c_str() , strlen(requestStr.c_str() ) + 1 , 0 ) ; 
    if(len == -1) {
        std::cout << "send error" << std::endl ;
        return ;  
    } 
}

// {"addgroup" , "加入群组,格式addgroup:groupid"} 
void addgroup(int fd , std::string commandMessage ) {

    // 拆分参数
    int  groupid = atoi(commandMessage.c_str() )  ;  

    // 构建json字符串
    nlohmann::json request ; 
    request["msgid"] =  ADD_GROUP_MSG ; 
    request["id"] = globalCurrUser.getId() ; 
    request["groupid"] = groupid ;  

    std::string requestStr = request.dump() ;  

    // 发送数据
    int len = send(fd , requestStr.c_str() , strlen(requestStr.c_str() ) + 1 , 0 ) ; 
    if(len == -1) {
        std::cout << "send error" << std::endl ;
        return ;  
    } 
}

// {"groupchat" , "群聊, 格式groupchat:groupid:message"} 
void groupchat(int fd , std::string commandMessage ) {

    // 拆分参数
    int idx = commandMessage.find(':') ; 
    if(idx == std::string::npos ) {
        std::cout << "args error" << std::endl ; 
        return ; 
    } 
    int groupid = atoi(commandMessage.substr(0 , idx ).c_str() ) ; 
    std::string message = commandMessage.substr(idx + 1) ; 

    // 构建json字符串
    nlohmann::json request ; 
    request["msgid"] =  GROUP_CHAT_MSG ; 
    request["id"] = globalCurrUser.getId() ; 
    request["name"] = globalCurrUser.getName() ; 
    request["groupid"] = groupid ; 
    request["msg"] = message ; 
    request["time"] = getCurrentTime() ; 

    std::string requestStr = request.dump() ;  

    // 发送数据
    int len = send(fd , requestStr.c_str() , strlen(requestStr.c_str() ) + 1 , 0 ) ; 
    if(len == -1) {
        std::cout << "send error" << std::endl ;
        return ;  
    } 

}



// {"loginout" , "注销,格式loginout"}  , 在这个函数中 commandMessage 参数无用
void loginout(int fd , std::string commandMessage ) {
    
    // 构建json字符串
    nlohmann::json request ; 
    request["msgid"] =  LOGINOUT_MSG ; 
    request["id"] = globalCurrUser.getId() ; 

    std::string requestStr = request.dump() ;  

    // 发送数据
    int len = send(fd , requestStr.c_str() , strlen(requestStr.c_str() ) + 1 , 0 ) ; 
    if(len == -1) {
        std::cout << "send error" << std::endl ;
        return ;  
    } 

    // 这个时候应该退出主菜单，回到main函数的页面中
    LOGINOUT = true ; 

    
}

