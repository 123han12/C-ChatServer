#ifndef GROUP_H 
#define GROUP_H

#include <string> 
#include <vector>
#include <groupuser.hpp>
class Group {
public:
    Group(int id = -1 , std::string name = "" , std::string desc = "") {
        this->id = id ; 
        this->groupname = name ; 
        this->groupdesc = desc ; 
    }
    void setId(int id ) {this->id = id ; } 
    void setGroupname(std::string name ) {this->groupname = name ; }  
    void setGroupDesc(std::string desc ) {this->groupdesc = desc ; }
    std::vector<GroupUser>& getUsers() {return this->users ; }

    int getId() {return id ; } 
    std::string getGroupname() {return groupname ; }
    std::string getGroupdesc() {return groupdesc ; }  
    
private:    
    int id ; 
    std::string groupname ; 
    std::string groupdesc ; 
    std::vector<GroupUser>  users ; 
} ; 



#endif 