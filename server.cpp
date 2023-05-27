#include<sys/socket.h>
#include<stdio.h>//perror
#include<stdlib.h>//exit
#include<arpa/inet.h>//sockaddr_in
#include<cstring>//bzero

int main(){
    //IP地址采用IPv4；数据传输方式，SOCK_STREAM多用于TCP；协议
    //创建一个套接字sockfd，对外提供一个网络通信接口
    //Linux系统中，网络连接属于一个文件，用文件描述符表示
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1){
        perror("socket");
        exit(1);//异常退出
    }

    //对于客户端，服务器存在的唯一标识是一个IP地址和端口
    //我们将sockfd与IP地址和端口绑定
    //先创建一个sockaddr_in结构体（IPv4专用结构体）
    struct sockaddr_in serv_addr;
    //在使用前，需要将对象清空
    bzero(&serv_addr,sizeof(serv_addr));

    //设置地址族，IP地址和端口
    serv_addr.sin_family = AF_INET;//IPv4
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//本机
    serv_addr.sin_port = htons(9999);//端口号9999，转换为网络字节序

    //将socket地址与文件描述符绑定
    //所有专用socket地址在实际使用时都要转换为通用socket类型sockaddr
    //因为所有socket编程接口使用的地址参数类型都是sockaddr*
    int bindfd = bind(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr));
    if(bindfd == -1){
        perror("bind");
        exit(1);
    }

    //使用listen监听指定套接字，并指定套接字长度
    int listenfd = listen(sockfd,SOMAXCONN);
    if(listenfd == -1){
        perror("listen");
        exit(1);
    }

    //监听后，需要accep接受客户端请求
    //对于每个客户端，在接受连接同时要保存客户端的socket地址信息
    //accept会阻塞当前程序，直到有一个客户端socket被接受后程序才会往下运行
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr,sizeof(client_addr));
    int client_sockfd = accept(sockfd,(sockaddr*)&client_addr,&client_addr_len);
    if(client_sockfd == -1){
        perror("accept");
        exit(1);
    }
    printf("new client fd %d \n IP:%s \n Port:%d \n",client_sockfd,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

}
