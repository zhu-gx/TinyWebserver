#pragma once

#include<sys/epoll.h>
#include<vector>
#include<unistd.h>
#include<cstring>
#include"errif.h"

#define MAX_EVENTS 1024

class Epoll{
private:
    int epfd;
    struct epoll_event *events;
public:
    Epoll();
    ~Epoll();
    void addFd(int fd,uint32_t op);
    std::vector<epoll_event> poll(int timeout = -1);
};

Epoll::Epoll() : epfd(-1),events(nullptr){
    epfd = epoll_create1(0);//创建epoll文件描述符
    errif(epfd == -1,"epoll create error");
    events = new epoll_event[MAX_EVENTS];//设定epoll最大事件大小
    bzero(events,sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll(){
    if(epfd != -1){
        close(epfd);
        epfd = -1;
    }
    delete [] events;
}

void Epoll::addFd(int fd,uint32_t op){
    struct epoll_event ev;
    bzero(&ev,sizeof(ev));
    ev.data.fd = fd;//该IO口为新添加fd
    ev.events = op;//触发模式
    //将服务器fd添加到epoll（epfd）
    errif(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)==-1,"epoll add event error");
}

std::vector<epoll_event> Epoll::poll(int timeout){
    //不断监听epoll上的事件并处理
    std::vector<epoll_event> activeEvents;
    //有nfds个fd发生事件
    int nfds = epoll_wait(epfd,events,MAX_EVENTS,timeout);
    errif(nfds == -1,"epoll wait error");
    for(int i = 0;i < nfds;i++){//处理这nfds个事件
        //有事件触发的放入activeEvents数组
        activeEvents.push_back(events[i]);
    }
    return activeEvents;
}
