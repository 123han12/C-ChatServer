#include "user.hpp"

    
void User::setId(int id ) {
    this->_id = id ; 
} 
void User::setName(std::string name )  {
    this->_name = name ; 
}
void User::setPwd(std::string password ) {
    this->_password = password ; 
}
void User::setState(std::string stat) {
    this->_state = stat ; 
}

int User::getId() {
    return this->_id ; 
}
std::string User::getName() {
    return this->_name ; 
}
std::string User::getPwd() {
    return this->_password ; 
}   
std::string User::getState() {
    return this->_state ; 
}
