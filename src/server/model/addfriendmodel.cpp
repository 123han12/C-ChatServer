#include "addfriendmodel.hpp"
#define MAX_SQL_SIZE 1024 
#include "db.h"

void addFriendModel::insert(int userid , int friendid) {
    char sql[MAX_SQL_SIZE] = {'\0'} ; 
    char sql2[MAX_SQL_SIZE] = {'\0'} ; 

    sprintf(sql , "insert into friend values(%d , %d)" , userid , friendid ) ; 
    sprintf(sql2 , "insert into friend values(%d , %d)" , friendid , userid) ; 

    MySQL mysql ; 
    if(mysql.connect() ) {
        mysql.update(std::string(sql) ) ;  
        mysql.update(std::string(sql2) ) ;
    }

}

std::vector<User> addFriendModel::query(int userid) {
    // 根据 userid 找到好友的 name state id ,  涉及到 user 表和 friend 的两个表的多表联查
    char sql[MAX_SQL_SIZE] = {'\0'} ; 

    sprintf(sql ,
    "select user.id , user.name , user.state from user inner join friend on friend.friendid = user.id where friend.userid = %d" 
    , userid ) ; 

    std::vector<User> vec ; 
    MySQL mysql ; 
    if(mysql.connect() ) {
        MYSQL_RES* res = mysql.query(std::string(sql)) ; 
        if(res != nullptr ) {
            MYSQL_ROW row ; 
            while((row = mysql_fetch_row(res) ) != nullptr ) {   // 取出每一行
                User user ; 
                user.setId(atoi(row[0])) ;
                user.setName(std::string(row[1]));
                user.setState(std::string(row[2])) ; 
                vec.push_back(user) ;  
            }
        } 
    }
    return vec ; 
}