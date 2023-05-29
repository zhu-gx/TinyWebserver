#include<sys/socket.h>
#include<arpa/inet.h>//sockaddr_in
#include<cstring>//bzero
#include<unistd.h>//read&write&fcntl
#include<sys/epoll.h>//epoll
#include<fcntl.h>//fcntl
#include <stdio.h>
#include<errno.h>//errno
#include<vector>

#include"errif.h"//errif
#include"Epoll.h"
#include"InetAddress.h"
#include"Socket.h"


#define MAX_EVENTS 1024//epoll结构体最大事件大小
#define READ_BUFFER 1024//读数据的buf大小

void handleReadEvent(int);

int main(){
    Socket *server_socket = new Socket();
    InetAddress *server_addr = new InetAddress("127.0.0.1",9999);
    server_socket->bind(server_addr);
    server_socket->listen();
    Epoll *ep = new Epoll();
    server_socket->setnonblocking();
    ep->addFd(server_socket->getFd(),EPOLLIN | EPOLLET);
    while(true){
        std::vector<epoll_event> events = ep->poll();
        int nfds = events.size();
        for(int i = 0;i < nfds;i++){
            if(events[i].data.fd == server_socket->getFd()){
                //新客户端连接
                InetAddress *client_addr = new InetAddress();//没有delete，会发生内存泄漏
                Socket *client_socket = new Socket(server_socket->accept(client_addr));//内存泄漏
                printf("new client fd %d!\nIP:%s\nPort:%d\n",client_socket->getFd(),inet_ntoa(client_addr->addr.sin_addr),ntohs(client_addr->addr.sin_port));
                client_socket->setnonblocking();
                ep->addFd(client_socket->getFd(),EPOLLIN|EPOLLET);
            }else if(events[i].events & EPOLLIN){//可读事件
                handleReadEvent(events[i].data.fd);
            }else{
                //其他事件，之后的版本实现
                printf("sonething else happened\n");
            }
        }
    }
    delete server_socket;
    delete server_addr;
    return 0;
}

void handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){
        //由于使用非阻塞IO，ET触发模式，只有在事件变化时才会收到通知
        //2k的数据如果只读1k，将不会受到通知还有1k未读取
        //因此需要一次读取buf大小数据，直到全部读取完毕
        bzero(&buf,sizeof(buf));
        ssize_t bytes_read = read(sockfd,buf,sizeof(buf));
        if(bytes_read > 0){
            //正常读取
            printf("message from client fd %d:%s\n",sockfd,buf);
            write(sockfd,buf,sizeof(buf));
        }else if(bytes_read == -1 && errno == EINTR){
            //客户端正常中断，继续读取
            printf("continue reading");
            continue;
        }else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            //非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        }else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}