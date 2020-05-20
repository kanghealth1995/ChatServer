/*
muduo网络库给用户提供的两个主要的类
TcpServer:用于编写服务器程序的
TcpClient:用于编写客户端程序的

epoll+线程池
好处：能够把网络I/O的代码和业务代码区分开
        用户的连接和断开        用户的可读写事件
 */
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<functional>
#include<iostream>
#include<string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;//占位符
/*基于muduo网络库开发服务器程序
1.创建TCPServer对象
2.创建EventLoop事件循环对象的指针；
3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4.在当前服务器类的构造函数中，注册处理连接的回调函数和消息处理回调函数
5.设置合适的服务端线程数量,muduo库会自己去分配I/O线程和worker线程
 */

class ChatServer
{
public:
    ChatServer(EventLoop* loop,//事件循环，反应堆
            const InetAddress& listenAddr,//IP+Port
            const string& nameArg)//服务器的名字
            : _server(loop,listenAddr,nameArg)
            ,_loop(loop)
    {
        //给服务器注册用户连接和创建和断开回调,其中_1为占位符
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
        //给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));

        //设置服务器端的线程数量 1个IO线程，3个worker线程
        _server.setThreadNum(4);
    }
    void start(){
        _server.start();
    }
private:
    //专门处理用户的连接创建和断开  epoll listenfd accept
    void onConnection(const TcpConnectionPtr& conn)
    {
        if(conn->connected()){
            cout<<conn->peerAddress().toIpPort()<<"->"<<
                conn->localAddress().toIpPort()<<"state:online"<<endl;
        }else{
            cout<<conn->peerAddress().toIpPort()<<"->"<<
                conn->localAddress().toIpPort()<<"state:offline"<<endl;
            conn->shutdown();//相当于close(fd)

        }
    }
    void onMessage(const TcpConnectionPtr& conn,
                    Buffer* buffer,
                    Timestamp receiveTime)
    {
        string buf = buffer->retrieveAllAsString();
        cout<<"recv data:"<<buf<<"time:"<<receiveTime.toFormattedString()<<endl;
        conn->send(buf);
    }

    TcpServer _server;  //#1
    EventLoop *_loop;   //#2 epoll
    

};

int main()
{
    EventLoop loop;//epoll
    InetAddress addr("127.0.0.1",6000);
    ChatServer server(&loop,addr,"chat_server");

    server.start();
    loop.loop();//相当于epoll_wait以阻塞的方式等待新用户连接，已连接用户的读写事件等
    return 0;
}