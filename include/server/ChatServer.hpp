#ifndef CHATSERVER_H
#define CHATSERVER_H

#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    //初始化服务器对象
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    //启动服务
    void start();


private:
    //上报链接相关信息的回调函数
    void onConnection(const TcpConnectionPtr&);
    //报文相关的回调函数
    void onMessage(const TcpConnectionPtr&,
                            Buffer*,
                            Timestamp);
    TcpServer _server;//组合的muduo库，实现服务器
    EventLoop *_loop;

};

#endif
