#include"offlinemessagemodel.hpp"
#include"db.hpp"

#include<string>
#include<mysql/mysql.h>
#include<vector>

//存储用户离线消息
void OfflineMessage::insert(int id,string msg)
{
    char sql[1024]={0};
    sprintf(sql,"insert into offlinemessage values(%d,'%s')",id,msg.c_str());

    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

//删除用户的离线消息
void OfflineMessage::remove(int id)
{
    char sql[1024]={0};
    sprintf(sql,"delete from offlinemessage where userid=%d",id);

    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}

//查询用户的离线消息
vector<string> OfflineMessage::query(int id)
{
    vector<string> vec;
    char sql[1024]={0};
    sprintf(sql,"select message from offlinemessage where userid = %d",id);

    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res!=nullptr){
            MYSQL_ROW row;
            while((row= mysql_fetch_row(res))!=nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
