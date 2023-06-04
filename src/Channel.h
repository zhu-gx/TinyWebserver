#pragma once
#include <sys/epoll.h>
#include <functional>
class Socket;
class EventLoop;
class Channel{
private:
    EventLoop *loop;
    int fd;
    uint32_t events;
    uint32_t ready;
    bool inEpoll;
    //bool useThreadPool;
    std::function<void()> readcallback;
    std::function<void()> writecallback;
public:
    Channel(EventLoop *_loop, int _fd);
    ~Channel();

    void handleEvent();
    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll(bool _in = true);
    void useET();

    // void setEvents(uint32_t);
    void setReady(uint32_t);
    void setReadCallback(std::function<void()>);
    //void setUseThreadPool(bool use = true);
};

