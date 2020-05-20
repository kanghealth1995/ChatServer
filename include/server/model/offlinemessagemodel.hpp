#ifndef OFFLINEMESSAGE_HPP
#define OFFLINEMESSAGE_HPP

#include<string>
#include<vector>
using namespace std;

//提供离线消息的接口和方法
class OfflineMessage
{
public:
    //存储用户离线消息
    void insert(int id,string msg);

    //删除用户的离线消息
    void remove(int id);

    //查询用户的离线消息
    vector<string> query(int id);
};


#endif
