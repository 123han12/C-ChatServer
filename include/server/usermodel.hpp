#ifndef USERMODEL_H 
#define USERMODEL_H 
#include "db.h"
#include "user.hpp"
#include <iostream>
#include <cstdio>
#include <string.h>

#define MAX_SQL_SIZE      1024 

class UserModel{
public:
    bool insert(User& user ) { // 向数据库中插入user 对象所携带的语句
        char sql[MAX_SQL_SIZE]  = {'\0' }; 
        sprintf(sql , "insert into user(name , password , state) values('%s' , '%s' , '%s')" , 
            user.getName().c_str() , user.getPwd().c_str() , user.getState().c_str() 
        ) ; 
        MySQL mysql ; 

        if(mysql.connect() ) { // 连接成功
            if(mysql.update(sql) ) {
                user.setId(mysql_insert_id(mysql.getConnection() )) ; // 将mysql 为其分配的主键id给到上层的user中 
                return true ; 
            } 
        }   

        return false ;
    }

    // 查询数据库中是否存在对应的用户
    User query(int id) {

        char sql[MAX_SQL_SIZE]  = {'\0' }; 
        sprintf(sql , "select * from user where id = %d" , id ) ;  
        MySQL mysql ; 
        if(mysql.connect() ) { // 连接成功
            MYSQL_RES* res = mysql.query(std::string(sql) ) ; // 获取查询结果集

            if(res != nullptr ) {
                MYSQL_ROW row = mysql_fetch_row(res) ;  // MYSQL_ROW 实际上就是二级指针
                if(row != nullptr ) {
                    User user ; 
                    user.setId(atoi(row[0]) ) ; 
                    user.setName(row[1]);
                    user.setPwd(row[2]) ; 
                    user.setState(row[3]) ;  
                    mysql_free_result(res) ; 
                    return user ; 
                }
            } 
        }   
        return User() ; 
    } 

    bool updateState(User user ) {
        char sql[MAX_SQL_SIZE]  = {'\0'}; 
        int id = user.getId() ; 
        sprintf(sql , "update user set state = 'online' where id = %d" , id ) ;  

        MySQL mysql ; 
        if(mysql.connect() ) { // 连接成功
            if(mysql.query(std::string(sql)) ) {
                return true ; 
            }
        }   
        return false ; 
    }


} ; 


#endif 