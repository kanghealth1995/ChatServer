#include "ChatServer.hpp"
#include "ChatService.hpp"
#include "json.hpp"

#include<functional>
#include<iostream>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg)
    :_server(loop,listenAddr,nameArg), _loop(loop)
{
    //注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
    //注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));
    //设置线程数量
    _server.setThreadNum(4);

}

void ChatServer::start()
{
    _server.start();
}

//上报链接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    if(!conn->connected()){
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}
//报文相关的回调函数
void ChatServer::onMessage(const TcpConnectionPtr& conn,
                Buffer* buffer,
                Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    cout<<buf<<endl;
    //数据的反序列化
    json js=json::parse(buf);
    //完全解耦合网络模块和业务模块的代码
    //通过js["msgid"]获取业务handler
    auto msgHandler=ChatService::instance()->getHandler(js["msgid"].get<int>());//将json类型转换为int 类型
    //回调消息绑定好的事件处理器，来执行相应的业务处理
    msgHandler(conn,js,time);
}
