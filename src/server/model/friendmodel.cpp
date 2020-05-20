#include "friendmodel.hpp"
#include "db.hpp"

//添加好友
void friendModel::insert(int userid,int friendid)
{
    char sql[1024]={0};
    sprintf(sql,"insert into friend values(%d,%d)",userid,friendid);

    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

//返回好友列表
vector<User> friendModel::query(int userid)
{
    char sql[1024]={0};
    sprintf(sql,"select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d",userid);
    
    vector<User> vec;
    
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr)
        {
            //把userid用户的所有朋友信息返回
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res))!=nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
