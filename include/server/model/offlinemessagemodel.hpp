#ifndef OFFLINEMESSAGEMODEL_H 
#define OFFLINEMESSAGEMODEL_H
#include <string>
#include <vector>

#define MAX_SQL_SIZE      1024 

class OfflineMessageModel{
public:
    void insert(int userid , std::string offlinemessage ) ; 
    
    void remove(int userid ) ; // 移除发送给用户userid的所有的离线的消息

    std::vector<std::string> query(int userid) ; 

} ; 


#endif 