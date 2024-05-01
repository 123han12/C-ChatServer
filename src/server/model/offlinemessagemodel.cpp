#include "offlinemessagemodel.hpp"
#include "db.h"


void OfflineMessageModel::insert(int userid , std::string offlinemessage ) {
    char sql[MAX_SQL_SIZE] = {'\0'} ; 

    sprintf(sql , "insert into offlinemessage values(%d , '%s')" , userid , offlinemessage.c_str() ) ; 

    MySQL mysql ; 
    if(mysql.connect() ) {
        mysql.update(std::string(sql)) ; 
    }
}

void OfflineMessageModel::remove(int userid ){ // 移除发送给用户userid的所有的离线的消息
    char sql[MAX_SQL_SIZE] = {'\0'} ; 

    sprintf(sql , "delete from offlinemessage where userid = %d" , userid ) ; 

    MySQL mysql ; 
    if(mysql.connect() ) {
        mysql.update(std::string(sql)) ; 
    }

}

std::vector<std::string> OfflineMessageModel::query(int userid) {
    char sql[MAX_SQL_SIZE] = {'\0'} ; 

    sprintf(sql , "select message from offlinemessage where userid = %d" , userid ) ; 

    std::vector<std::string> vec ; 
    MySQL mysql ; 
    if(mysql.connect() ) {
        MYSQL_RES* res = mysql.query(std::string(sql)) ; 
        if(res != nullptr ) {
            MYSQL_ROW row ; 
            while((row = mysql_fetch_row(res) ) != nullptr ) {   // 取出每一行
                vec.push_back(std::string(row[0])) ; 
            }
        } 
    }
    return vec ; 
}