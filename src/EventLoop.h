#pragma once
#include<functional>

//这是一个事件循环
//我们添加需要监听的事务到这个事件循环内
//每次有事件发生就会通知
//程序中返回给Channel
//然后根据不同的描述符、事件类型进行处理（以回调函数的方式）
class Epoll;
class Channel;
class ThreadPool;
class EventLoop
{
private:
    Epoll *ep;
    bool quit;
    ThreadPool *threadPool;
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void updateChannel(Channel*);
    void addThread(std::function<void()>);
};

