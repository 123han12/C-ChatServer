#ifndef GROUPUSER_H 
#define GROUPUSER_H
#include <string>
#include <user.hpp>


// 主要用于 GroupUser 表格
class GroupUser : public User
{
public:
    void setRole(std::string role ) {this->role = role ; } ; 
    std::string getRole() {return this->role ; } ; 
private:
    std::string role; 
} ; 


#endif 