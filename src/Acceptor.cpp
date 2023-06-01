#include"Acceptor.h"
#include"Socket.h"
#include"InetAddress.h"
#include"Channel.h"
#include<stdio.h>

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop){
    sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1",9999);
    sock->bind(addr);
    sock->listen();
    sock->setnonblocking();
    acceptChannel = new Channel(loop,sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection,this);
    acceptChannel->setCallback(cb);
    acceptChannel->enableReading();
}

Acceptor::~Acceptor(){
    delete sock;
    delete acceptChannel;
}

void Acceptor::acceptConnection(){
    InetAddress *client_addr = new InetAddress();
    Socket *client_sock = new Socket(sock->accept(client_addr));
    printf("new client fd:%d!IP:%s\nPort:%d\n",client_sock->getFd(),inet_ntoa(client_addr->getAddr().sin_addr),ntohs(client_addr->getAddr().sin_port));
    client_sock->setnonblocking();
    newConnectionCallback(client_sock);
    delete client_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb){
    newConnectionCallback = _cb;
}