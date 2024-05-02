#include "groupmodel.hpp"
#include "db.h"
#include <vector>

#define MAX_SQL_SIZE 1024 


// 根据 group 传入的信息，在数据库中创建一个群组
bool GroupModel::createGroup(Group& group ) {
    char sql[MAX_SQL_SIZE] ; 
    sprintf(sql , "insert into allgroup(`groupname` , `groupdesc`) values('%s' , '%s')" , 
        group.getGroupname().c_str() , group.getGroupdesc().c_str() 
    ) ; 
    
    MySQL mysql ; 
    if(mysql.connect() ) {
        if(mysql.update(std::string(sql)) ) {
            int groupid = mysql_insert_id(mysql.getConnection() ) ; 
            group.setId(groupid) ;  
            return true ; 
        }
    }    
    return false ; 
}

// 将 userid 的用户以 role 的身份添加到群组 groupid 中
void GroupModel::addGroup(int userid , int groupid , std::string role){
    char sql[MAX_SQL_SIZE] ; 
    sprintf(sql , "insert into groupuser values('%d' , '%d' , '%s')" ,  
        groupid , userid , role.c_str() 
    ) ;  
    MySQL mysql ; 
    if(mysql.connect() ) {
        mysql.update(std::string(sql))  ; 
    }    
}




// 查询用户所在的所有群组的信息, 并将每一个群组的所有组员的信息都存储到Group中
/*
    1.查询出用户所在的所有的组的信息，
    2.根据对应组的信息填充组的组员的信息 
*/
std::vector<Group> GroupModel::queryGroups(int userid ){
    char sql[MAX_SQL_SIZE] ; 

    // 找到该userid 所对应的所有的组的信息
    sprintf(sql , 
    "select a.id , a.groupname , a.groupdesc from allgroup as a inner join groupuser as b on a.id = b.groupid where b.userid = %d"
     , userid 
    ) ; 
    MySQL mysql ; 
    std::vector<Group> groupVec ;  
    if(mysql.connect() ) {
        MYSQL_RES* res = mysql.query(std::string(sql) ) ; 
        if(res != nullptr ) {
            MYSQL_ROW row ; 
            while((row = mysql_fetch_row(res) ) ) {
                Group group ; 
                group.setId(atoi(row[0])) ;
                group.setGroupname(row[1]) ; 
                group.setGroupDesc(row[2] ) ; 
                groupVec.push_back(group) ;   
            }
            mysql_free_result(res) ; 
        }
    }

    // 对于该用户的每一个组都需要获取其所有的组员的信息
    for(Group& group : groupVec ) {
        int groupid = group.getId() ; 

        // 找到groupid 所有组员的详细信息
        sprintf(sql ,
         "select a.id , a.name , a.state , b.grouprole from user as a inner join groupuser as b on b.userid = a.id where b.groupid = %d"
         , groupid 
        ) ; 
        
        MYSQL_RES* res = mysql.query(std::string(sql) ) ; 
        if(res != nullptr ) {
            MYSQL_ROW row ;

            std::vector<GroupUser>& userVec = group.getUsers() ;   
            while((row = mysql_fetch_row(res) ) ) {
                GroupUser user ; 
                user.setId(atoi(row[0] ) ) ; 
                user.setName(row[1] ) ; 
                user.setState(row[2] ) ; 
                user.setRole(row[3] ) ;  

                userVec.push_back(user) ; 
            }
            mysql_free_result(res) ; 
        }
    
    }   

    return groupVec ; 
}



std::vector<int> GroupModel::queryGroupUsers(int userid , int groupid ){
    char sql[MAX_SQL_SIZE] ; 
    sprintf(sql , "select userid from groupuser where groupid = %d and userid != %d" , groupid , userid ) ;  
    
    MySQL mysql ;
    std::vector<int> useridVec ;  
    if(mysql.connect() ) {
        MYSQL_RES* res = mysql.query(std::string(sql)) ; 
        MYSQL_ROW row ; 
        while((row = mysql_fetch_row(res) ) != nullptr ) {
            useridVec.push_back(atoi(row[0] ) ) ; 
        }
    } 
    
    return useridVec ; 
}