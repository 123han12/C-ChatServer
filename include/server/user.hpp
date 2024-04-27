#ifndef USER_H 
#define USER_H 

#include <string>

/*
ORM（Object-Relational Mapping）是一种以对象的方式来操作数据库的技术，是数据库操作的新方向。 
使开发者可以用面向对象的方式操作数据库，通过ORM技术，可以提高开发效率、减少错误、提高代码可读性。 
但也存在性能问题，不适合复杂的数据库操作。
*/

class User{

public:
    User(int id = -1 , std::string name = "" , std::string pwd = "" , std::string stat = "offline"){
        this->_id = id ; 
        this->_name = name ; 
        this->_password = pwd ; 
        this->_state = stat ; 
    }
    void setId(int id ) ; 
    void setName(std::string name ) ; 
    void setPwd(std::string password ) ; 
    void setState(std::string stat) ; 

    int getId() ; 
    std::string getName() ; 
    std::string getPwd() ; 
    std::string getState() ; 

private:
    int _id ; 
    std::string _name ;
    std::string _password ; 
    std::string _state ; 

} ; 


#endif 