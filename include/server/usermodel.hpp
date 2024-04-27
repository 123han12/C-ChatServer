#ifndef USERMODEL_H 
#define USERMODEL_H 
#include "db.h"
#include "user.hpp"
#include <iostream>
#include <cstdio>

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


} ; 


#endif 