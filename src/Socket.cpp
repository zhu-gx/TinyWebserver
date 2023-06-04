#include "Socket.h"
#include "errif.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>

Socket::Socket() : fd(-1){
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}
Socket::Socket(int _fd) : fd(_fd){
    errif(fd == -1, "socket create error");
}

Socket::~Socket(){
    if(fd != -1){
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress *_addr){
    struct sockaddr_in addr = _addr->getAddr();
    //socklen_t addr_len = _addr->getAddr();
    errif(::bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1, "socket bind error");
    //_addr->setInetAddr(addr,addr_len);
}

void Socket::listen(){
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}
void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *_addr){
    int client_sockfd = -1;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t addr_len = sizeof(addr);
    if(fcntl(fd,F_GETFL) & O_NONBLOCK){
        while(true){
            client_sockfd = ::accept(fd,(sockaddr*)&addr,&addr_len);
            if(client_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                continue;
            }else if(client_sockfd == -1){
                errif(true,"socket accept error");
            }else{
                break;
            }
        }
    }else{
        client_sockfd = ::accept(fd,(sockaddr*)&addr,&addr_len);
        errif(client_sockfd == -1,"socket accept error");
    }
    _addr->setInetAddr(addr);
    return client_sockfd;
}

void Socket::connect(InetAddress *_addr){
    struct sockaddr_in addr = _addr->getAddr();
    //socklen_t addr_len = _addr->getAddr();
    if(fcntl(fd,F_GETFL) & O_NONBLOCK){
        while(true){
            int ret = ::connect(fd,(sockaddr*)&addr,sizeof(addr));
            if(ret == 0){
                break;
            }else if(ret == -1 && (errno == EINPROGRESS)){
                continue;
            }else if(ret == -1){
                errif(true,"socket connect error");
            }
        }
    }else{
        errif(::connect(fd,(sockaddr*)&addr,sizeof(addr))==-1,"socket connect error");
    }
}

int Socket::getFd(){
    return fd;
}


InetAddress::InetAddress() {
    bzero(&addr, sizeof(addr));
}
InetAddress::InetAddress(const char* _ip, uint16_t _port) {
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(_ip);
    addr.sin_port = htons(_port);
}

InetAddress::~InetAddress(){
}

void InetAddress::setInetAddr(sockaddr_in _addr){
    addr = _addr;
}

sockaddr_in InetAddress::getAddr(){
    return addr;
}

char* InetAddress::getIp(){
    return inet_ntoa(addr.sin_addr);
}

uint16_t InetAddress::getPort(){
    return ntohs(addr.sin_port);
}