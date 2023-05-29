#pragma once
#include<sys/epoll.h>

#include"Epoll.h"

//使用channel
//不仅可以通过文件描述符获取事件发生
//还可以获得文件描述符的连接类型(HTTP\FTP\...)

class Epoll;
class Channel{
private:
    Epoll *ep;//文件描述符分发到的Epoll类
    int fd;//该Channel负责的文件描述符
    uint32_t events;//目标监听fd的哪个事件
    uint32_t revents;//epoll返回该Channel时fd正在发生的事件
    bool inEpoll;//当前Channel是否在epoll中
public:
    Channel(Epoll *_ep,int _fd);
    ~Channel();

    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();

    //void setEvents(uint32_t);
    void setRevents(uint32_t);
};

Channel::Channel(Epoll *_ep,int _fd) 
: ep(_ep),fd(_fd),events(0),revents(0),inEpoll(false){

}

Channel::~Channel(){

}

void Channel::enableReading(){
    events = EPOLLIN | EPOLLET;//将要监听的事件events设为读模式并采用ET模式
    ep->updateChannel(this);//向ep指针指向的Epoll中更新该Channel
}

int Channel::getFd(){
    return fd;
}

uint32_t Channel::getEvents(){
    return events;
}

uint32_t Channel::getRevents(){
    return revents;
}

bool Channel::getInEpoll(){
    return inEpoll;
}

void Channel::setRevents(uint32_t _ev){
    revents = _ev;
}