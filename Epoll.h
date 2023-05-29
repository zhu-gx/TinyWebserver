#pragma once

#include<sys/epoll.h>
#include<vector>
#include<unistd.h>
#include<cstring>
#include"errif.h"
#include"Channel.h"

#define MAX_EVENTS 1024

class Epoll{
private:
    int epfd;
    struct epoll_event *events;
public:
    Epoll();
    ~Epoll();
    void addFd(int fd,uint32_t op);
    void updateChannel(Channel*);
    std::vector<Channel*> poll(int timeout = -1);
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

//使用时，可以通过poll()获取当前有事件发生的Channel
std::vector<Channel*> Epoll::poll(int timeout){
    //不断监听epoll上的事件并处理
    std::vector<Channel*> activeEvents;
    //有nfds个fd发生事件
    int nfds = epoll_wait(epfd,events,MAX_EVENTS,timeout);
    errif(nfds == -1,"epoll wait error");
    for(int i = 0;i < nfds;i++){//处理这nfds个事件
        //有事件触发的放入activeEvents数组
        Channel *ch = (Channel*)events[i].data.ptr;
        ch->setRevents(events[i].events);
        activeEvents.push_back(ch);
    }
    return activeEvents;//所有事件发生的Channel
}

void Epoll::updateChannel(Channel *channel){
    int fd = channel->getFd();//拿到Channel的文件描述符
    struct epoll_event ev;
    bzero(&ev,sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();//拿到Channel希望监听的事件
    if(!channel->getEvents()){
        errif(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev)==-1,"epoll add error");//添加Channel中的fd到epoll
        channel->setInEpoll();
    }else{
        errif(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev)==-1,"epoll modify error");//已存在，则修改
    }
}
