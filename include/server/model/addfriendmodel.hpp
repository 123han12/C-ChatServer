#ifndef ADDFRIENDMODEL_H 
#define ADDFRIENDMODEL_H
#include <vector>
#include <user.hpp>

class addFriendModel{
public:
    void insert(int userid , int friendid) ; 
    
    std::vector<User> query(int userid) ;  

} ; 

#endif 