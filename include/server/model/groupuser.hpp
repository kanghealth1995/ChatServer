#ifndef GROUPUSER_HPP
#define GROUPUSER_HPP
#include"user.hpp"
#include<string>

//群组用户，继承于User，多了个role属性
class GroupUser :public User
{
public:
    void setRole(string role){this->role = role;}
    string getRole(){return this->role;}
private:
    string role;
};

#endif