#include"json.hpp"
using json = nlohmann::json;

#include<iostream>
#include<vector>
#include<string>
#include<map>
using namespace std;

//json 序列化示例1
void func1()
{
    json js;
    js["msg_type"] =2;
    js["from"] ="zhangsan";
    js["to"] ="lisi";
    js["msg"] ="hello lisi,where are you from?";
    string SendBuf=js.dump();
     cout<<js<<endl;
     cout<<SendBuf.c_str()<<endl;
}

//json 序列化示例2
void func2()
{
    json js;
    // 添加数组
    js["id"] = {1,2,3,4,5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};
    cout << js << endl;
}

//json 序列化示例3 容器序列化
void func3()
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
    string SendBuf=js.dump();
    cout<<js<<endl;
    cout<<SendBuf.c_str()<<endl;
}
//json 反序列化示例1
string func4()
{
    json js;
    js["msg_type"] =2;
    js["from"] ="zhangsan";
    js["to"] ="lisi";
    js["msg"] ="hello lisi,where are you from?";
    string SendBuf=js.dump();
    // cout<<js<<endl;
    // cout<<SendBuf.c_str()<<endl;
    return SendBuf;
}

//json 反序列化示例2
string func5()
{
    json js;
    // 添加数组
    js["id"] = {1,2,3,4,5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};
    return js.dump();
}

//json 反序列化示例3 容器反序列化
string func6()
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
    string SendBuf=js.dump();
    return SendBuf;
}
int main()
{
    // func1();
    // func2();
    // func3();

    string revcBuf = func6();
    json jsBuf = json::parse(revcBuf);

    //test for func4
    // cout<<jsBuf["msg_type"]<<endl;
    // cout<<jsBuf["from"]<<endl;
    // cout<<jsBuf["to"]<<endl;
    // cout<<jsBuf["msg"]<<endl;

    //test for func5
    // cout<<jsBuf["id"]<<endl;
    // auto arr = jsBuf["id"];
    // cout<<arr[3]<<endl;
    // auto msgjs=jsBuf["msg"];
    // cout<<msgjs["zhang san"]<<endl;
    
    //test for func6
    vector<int> vec = jsBuf["list"];
    for(auto& v:vec)
        cout<<v<<" ";
    cout<<endl;
    map<int,string> mymap=jsBuf["path"];
    for(auto& a:mymap)
        cout<<a.first<<" "<<a.second<<endl;

    return 0;
}