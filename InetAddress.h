#pragma once

//该头文件用于设定sockaddr_in结构体（IPv4专用结构体）
//包括协议族、IP地址、端口号
#include<arpa/inet.h>//sockaddr_in
#include<cstring>//bzero

class InetAddress{
public:
    struct sockaddr_in addr;
    socklen_t addr_len;
    InetAddress();
    InetAddress(const char* ip,uint16_t port);
    ~InetAddress();
};

InetAddress::InetAddress() : addr_len(sizeof(addr)){
    bzero(&addr,sizeof(addr));
}

//设置地址族，IP地址和端口
//先创建一个sockaddr_in结构体（IPv4专用结构体）
InetAddress::InetAddress(const char* ip,uint16_t port) : addr_len(sizeof(addr)){
    bzero(&addr,sizeof(addr));//清空sockaddr_in结构体
    addr.sin_family = AF_INET;//IPv4
    addr.sin_addr.s_addr = inet_addr(ip);//指定IP地址
    addr.sin_port = htons(port);//端口号，转换为网络字节序
    addr_len = sizeof(addr);
}

InetAddress::~InetAddress(){}