#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include<muduo/net/TcpConnection.h>
#include<unordered_map>
#include<functional>
#include<mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "redis.hpp"
#include "groupmodel.hpp"
#include "friendmodel.hpp"
#include "offlinemessagemodel.hpp"
#include"usermodel.hpp"
#include "json.hpp"
using json = nlohmann::json;
//表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn,json &js, Timestamp time)>;
//聊天服务器业务类
class ChatService
{
public:
    //获取单例对象的接口函数
    static ChatService* instance();
    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    //处理登录业务
    void login(const TcpConnectionPtr &conn,json &js, Timestamp time);
    //处理注册业务
    void reg(const TcpConnectionPtr &conn,json &js, Timestamp time);
    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    //处理一对一聊天业务
    void p2pChat(const TcpConnectionPtr &conn,json &js, Timestamp time);
    //添加朋友业务
    void addFriend(const TcpConnectionPtr &connm,json &js,Timestamp time);
    //服务器异常退出，业务重置
    void reset();
    //用户登出
    void LoginOut(const TcpConnectionPtr &connm,json &js,Timestamp time);

    //群组业务相关
    //创建群组
    void createGroup(const TcpConnectionPtr &connm,json &js,Timestamp time);
    //群组添加用户
    void addGroup(const TcpConnectionPtr &connm,json &js,Timestamp time);
    //群组聊天
    void groupChat(const TcpConnectionPtr &connm,json &js,Timestamp time);
    //处理redis订阅消息
    void handleRedisSubscribeMessage(int userid, string msg);
private:
    ChatService();
    //存储消息id和其对应的事件业务处理方法
    unordered_map<int,MsgHandler> _msgHandlerMap;

    unordered_map<int,TcpConnectionPtr> _msgConnMap;

    mutex _connMutex;

    UserModel _usermodel;
    OfflineMessage _offlineMsgModel;
    friendModel _friendModel;
    GroupModel _groupModel; 
    Redis   _redis;
};

#endif
