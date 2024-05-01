#ifndef GROUPMODEL_H 
#define GROUPMODEL_H
#include "group.hpp"
#include <string> 
#include <vector> 

// 操作类，这里不包含数据成员
class GroupModel {
public:

    // 根据 group 传入的信息，在数据库中创建一个群组
    bool createGroup(Group& group ) ; 

    // 将 userid 的用户以 role 的身份添加到群组 groupid 中
    void addGroup(int userid , int groupid , std::string role) ;  

    // 查询用户所在的所有群组的信息
    std::vector<Group> queryGroups(int userid ) ; 

    // 查询userid 所在的groupid 的组中的除了 userid 的其余的用户的id 
    std::vector<int> queryGroupUsers(int userid , int groupid ) ; 
} ; 


#endif 