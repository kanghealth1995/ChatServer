#ifndef USERMODEL_HPP
#define USERMODEL_HPP

#include"user.hpp"
class UserModel
{
public:
    // User表的增加方法
    bool insert(User &user);
    User query(int id);
    bool updateState(User &user);
    void resetState(void);
};

#endif