#include <mysql/mysql.h>
#include <iostream>

// 测试当前api所使用的默认编码

int main() {
    MYSQL* mysql = mysql_init(nullptr) ;
    mysql = mysql_real_connect(mysql , "localhost" , "root" , "123456" , "chat" , 3306 , nullptr , 0 ) ; 
    std::cout << mysql_character_set_name(mysql) << std::endl ; 

    mysql_set_character_set(mysql , "gbk") ; 

    std::cout << mysql_character_set_name(mysql) << std::endl ; 

    while(1) {} 
    mysql_close(mysql) ; 


    return 0 ; 
}