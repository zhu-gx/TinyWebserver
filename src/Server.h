#pragma once

#include<map>
#include<vector>
//接受连接后，服务器将TCP保存在一个map中
//键为该连接客户端的socket fd，值为指向该连接的指针
class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server
{
private:
    EventLoop *mainReactor;//事件循环
    Acceptor *acceptor;//用于接受TCP连接
    std::map<int,Connection*> connections;//所有TCP连接
    std::vector<EventLoop*> subReactors;
    ThreadPool *thpool;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);//处理客户端请求
    void newConnection(Socket *serv_sock);//新建TCP连接
    void deleteConnection(int sock);//断开TCP连接
};

