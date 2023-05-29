#pragma once

//创建socket套接字
#include"InetAddress.h"
#include"errif.h"

#include<unistd.h>//read&write&fcntl
#include<fcntl.h>//fcntl
#include<sys/socket.h>//socket

class InetAddress;
class Socket{
private:
    int fd;
public:
    Socket();
    Socket(int);
    ~Socket();

    void bind(InetAddress*);
    void listen();
    void setnonblocking();

    int accept(InetAddress*);
    int getFd();
};

Socket::Socket() : fd(-1){
    //IP地址采用IPv4；数据传输方式，SOCK_STREAM多用于TCP；协议(TCP/UDP)
    //创建一个套接字sockfd，对外提供一个网络通信接口
    //Linux系统中，网络连接属于一个文件，用文件描述符表示
    fd = socket(AF_INET,SOCK_STREAM,0);
    errif(fd == -1,"socket create error");
}

Socket::Socket(int _fd) : fd(_fd){
    errif(fd == -1,"socket create error");
}

Socket::~Socket(){
    if(fd != -1){
        //关闭socket文件描述符
        close(fd);
        fd = -1;
    }
}

//对于客户端，服务器存在的唯一标识是一个IP地址和端口
//我们将sockfd与IP地址和端口绑定
//将socket地址与文件描述符绑定
//所有专用socket地址在实际使用时都要转换为通用socket类型sockaddr
//因为所有socket编程接口使用的地址参数类型都是sockaddr*
void Socket::bind(InetAddress *addr){
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket bind error");
}

//使用listen监听指定套接字，并指定套接字长度
void Socket::listen(){
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}
//将指定套接字设定为非阻塞
void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *addr){
    int clnt_sockfd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
    errif(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
}

int Socket::getFd(){
    return fd;
}
