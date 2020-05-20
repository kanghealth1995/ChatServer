#include "ChatService.hpp"
#include "public.hpp"
#include "user.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"
#include <muduo/base/Logging.h>
#include <vector>
#include <string>

using namespace std;
using namespace muduo;

/*
#include"ChatService.hpp"
#include"public.hpp"
//#include<muduo/base/Logging.h>

//using namespace std;
//using namespace muduo;
*/
//获取单例对象的接口函数
ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    //用户业务相关
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::p2pChat,this,_1,_2,_3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG,std::bind(&ChatService::addFriend,this,_1,_2,_3)});
    _msgHandlerMap.insert({LOGINOUT_MSG,std::bind(&ChatService::LoginOut,this,_1,_2,_3)});

    //群组业务相关
    _msgHandlerMap.insert({CREATE_GROUP_MSG,std::bind(&ChatService::createGroup,this,_1,_2,_3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG,std::bind(&ChatService::addGroup,this,_1,_2,_3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG,std::bind(&ChatService::groupChat,this,_1,_2,_3)});

    //连接redis服务器
    if(_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

MsgHandler ChatService::getHandler(int msgid)
{
    //记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if(it == _msgHandlerMap.end())
    {
        //返回一个默认的处理器，空操作
        return [=](const TcpConnectionPtr &conn,json &js, Timestamp time){
            LOG_ERROR <<"msgid:"<<msgid<<" can not find  handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

//处理登录业务
void ChatService::login(const TcpConnectionPtr &conn,json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    string pwd = js["password"];
    
    User user = _usermodel.query(id);
    if(user.getId()==id&&user.getPwd()==pwd)
    {
        if(user.getState()=="online"){
            //该用户已经登录，不允许重复登录
            json response;
            response["msgid"]=LOGIN_MSG_RES;
            response["errno"]=2;
            response["msg"]="用户已经在线,请换其他账号登录";
            conn->send(response.dump());
        }
        else{
            //登录成功,更新用户状态信息
            {//大括号是作用域，对象lock出了此作用域自动析构
                //实例化一个对象lock，在这个对象实例化的过程中，加锁
                lock_guard<mutex> lock(_connMutex);
                _msgConnMap.insert({id,conn});
                //在这个对象析构的过程中解锁
            }

            // id用户登录成功后，向redis订阅channel(id)
            _redis.subscribe(id); 

            //登录成功，更新用户的状态信息
            user.setState("online");
            _usermodel.updateState(user);

            json response;
            response["msgid"]=LOGIN_MSG_RES;
            response["errno"]=0;
            response["id"]=user.getId();
            response["name"]=user.getName();
            //查询这个用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if(!vec.empty())
            {
                response["offlinemsg"]=vec;
                _offlineMsgModel.remove(id);
            }

            //查询用户的朋友信息
            vector<User> userVec = _friendModel.query(id);
            if(!userVec.empty())
            {
                vector<string> vec2;
                for(User &user:userVec)
                {
                    json js;
                    js["id"]=user.getId();
                    js["name"]=user.getName();
                    js["state"]=user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"]=vec2;
            }

            //查询用户的群组信息
            vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }

                response["groups"] = groupV;
            }
            conn->send(response.dump());
        }
    }
    else
    {
        //登录失败
        json response;
        response["msgid"]=LOGIN_MSG_RES;
        response["errno"]=1;
        response["msg"]="用户名或者密码错误";
        conn->send(response.dump());

    }

}

//处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn,json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);

    bool state = _usermodel.insert(user);
    if(state)
    {
        //注册成功
        json response;
        response["msgid"]=REG_MSG_RES;
        response["errno"]=0;
        response["id"]=user.getId();
        conn->send(response.dump());
    }
    else
    {
        //注册失败
        json response;
        response["msgid"]=REG_MSG_RES;
        response["errno"]=1;
        conn->send(response.dump());
    }
}

//处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {//这是锁对象的作用域，超出作用域，锁对象析构，解锁
        lock_guard<mutex> lock(_connMutex);
        for(auto it =_msgConnMap.begin();it!=_msgConnMap.end();it++)
        {
            if(it->second==conn)
            {
                user.setId(it->first);
                //从map表中删除用户的连接信息
                _msgConnMap.erase(it);
                break;
            }
        }

        // 用户注销，相当于就是下线，在redis中取消订阅通道
        _redis.unsubscribe(user.getId()); 

        //更新用户状态信息
        if(user.getId()!=-1)
        {
            user.setState("offline");
            _usermodel.updateState(user);
        }
    }
}

//处理一对一聊天业务
void ChatService::p2pChat(const TcpConnectionPtr &conn,json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();//转换为int类型

    {//此大括号为了析构lock对象，从而解锁
        lock_guard<mutex> lock(_connMutex);
        auto it = _msgConnMap.find(toid);
        if(it!=_msgConnMap.end())
        {
            //对方在线
            it->second->send(js.dump());            
            return;
        }
    }
    // 查询toid是否在线 
    User user = _usermodel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump());
        return;
    }

    //对方不在线，则转存消息
    _offlineMsgModel.insert(toid,js.dump());

}

//添加朋友业务
void ChatService::addFriend(const TcpConnectionPtr &conn,json &js, Timestamp time)
{
    int friendid = js["friendid"].get<int>();
    int userid = js["id"].get<int>();

    //存储好友信息
    _friendModel.insert(userid,friendid);
    _friendModel.insert(friendid,userid);

}

//登出业务
void ChatService::LoginOut(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _msgConnMap.find(userid);
        if(it!=_msgConnMap.end())
        {
            _msgConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid); 
    //更新用户的状态信息
    User user;
    user.setId(userid);
    user.setState("offline");
    _usermodel.updateState(user);
}  

//服务器异常退出，业务重置
void ChatService::reset()
{
    //把online状态的用户，改为offline状态
    _usermodel.resetState();
}

//创建群组
void ChatService::createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}
//群组添加用户
void ChatService::addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid,groupid,"normal");
}
//群组聊天
void ChatService::groupChat(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
     int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec)
    {
        auto it = _msgConnMap.find(id);
        if (it != _msgConnMap.end())
        {
            // 转发群消息
            it->second->send(js.dump());
        }
        else
        {
            // 查询toid是否在线 
            User user = _usermodel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _msgConnMap.find(userid);
    if (it != _msgConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}
