#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

// json序列化示例1
string func1()
{
    json js;
    js["msg_type"] = 2;
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello , what are you doing now?";

    string sendBuf = js.dump();
    return sendBuf ; 
}
string func2()
{
    json js;
    // 添加数组
    js["id"] = {1, 2, 3, 4, 5}; // 添加数组
    // 添加key-value
    js["name"] = "zhangsan";
    // 添加对象
    js["msg"]["zhangsan"] = "helloworld";
    js["msg"]["liushuo"] = "hellochina";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhangsan", "helloworld"}, {"liushuo", "hellochina"}};
    return js.dump() ; 
}

// json序列化示例代码三, 序列化容器
void  func3()
{
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;
    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;
    cout << js << endl;
    string SendBuf = js.dump(); // json 数据对象 ---》 序列化json字符串。
    cout << SendBuf << endl;
}



int main()
{
    string recvBuf = func2() ; 

    json buf = json::parse(recvBuf) ;   // 对字符流进行反序列化 

    cout << buf["id"] << endl ; 
    auto arr = buf["id"] ; 
    for(auto& p : arr ) cout << p << ' ' ; 

    cout << arr[2] << endl ;  


    return 0;
}