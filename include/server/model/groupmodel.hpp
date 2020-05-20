#ifndef GROUPMODEL_HPP
#define GROUPMODEL_HPP

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

//维护群组信息的接口方法
class GroupModel
{
public:
    //创建群组
    bool createGroup(Group &group);
    //加入群组
    void addGroup(int userid,int groupid,string role);
    //查询用户所在群组信息
    vector<Group> queryGroups(int userid);
    //根据指定的groupid 查询群组用户列表，给其他成员发送消息
    vector<int> queryGroupUsers(int userid,int groupid);
};

#endif
