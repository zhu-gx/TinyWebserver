#pragma once
#include<functional>

//该类作用是将accept部分模块化
//该类存在于事件驱动EventLoop中


class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor{
private:
    EventLoop *loop;
    //类中的socketfd即服务器监听的socketfd
    //每一个acceptor对应一个socketfd
    Socket *sock;
    InetAddress *addr;
    //这个类也通过一个独有的Channel负责分发到epoll
    Channel *acceptChannel;
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    //新建连接的回调函数
    std::function<void(Socket*)> newConnectionCallback;
    void setNewConnectionCallback(std::function<void(Socket*)>);
};