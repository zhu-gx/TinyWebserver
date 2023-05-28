#include<sys/socket.h>
#include<stdio.h>//perror
#include<stdlib.h>//exit
#include<arpa/inet.h>//sockaddr_in
#include<cstring>//bzero
#include<unistd.h>//read&write&fcntl
#include<sys/epoll.h>//epoll
#include<fcntl.h>//fcntl
#include<errno.h>//errno

#define MAX_EVENTS 1024//epoll结构体最大事件大小
#define READ_BUFFER 1024//读数据的buf大小

void errif(bool condition,const char* errmsg);//错误处理函数
void setnonblocking(int fd);//将指定套接字设置为非阻塞

int main(){
    //IP地址采用IPv4；数据传输方式，SOCK_STREAM多用于TCP；协议
    //创建一个套接字sockfd，对外提供一个网络通信接口
    //Linux系统中，网络连接属于一个文件，用文件描述符表示
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    errif(sockfd == -1,"socket creat error");

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
    errif(bindfd == -1,"bind");

    //使用listen监听指定套接字，并指定套接字长度
    int listenfd = listen(sockfd,SOMAXCONN);
    errif(listenfd == -1,"socket listen error");

    //创建epoll文件描述符
    int epfd = epoll_create1(0);
    errif(epfd == -1,"epoll create error");

    struct epoll_event events[MAX_EVENTS],ev;
    bzero(&events,sizeof(events));

    bzero(&ev,sizeof(ev));
    ev.data.fd = sockfd;//该IO口为服务器sockefd
    //采用ET边缘触发模式，fd从无到有通知内核一次
    //必须搭配非阻塞式socket使用
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(sockfd);
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);//将服务器socketfd添加到epoll

    while(true){//不断监听epoll上的事件并处理
        int nfds = epoll_wait(epfd,events,MAX_EVENTS,-1);//有nfds个fd发生事件
        errif(nfds == -1,"epoll wait error");

        for(int i = 0;i < nfds;i++){//处理者nfds个事件
            if(events[i].data.fd == sockfd){//发生事件的fd是服务器socket fd，新客户端连接
                struct sockaddr_in client_addr;
                bzero(&client_addr,sizeof(client_addr));

                socklen_t client_addr_len = sizeof(client_addr);
                int client_sockfd = accept(sockfd,(sockaddr*)&client_addr,&client_addr_len);
                errif(client_sockfd == -1,"socket accept error");
                printf("new client fd %d! IP:%s \nPort:%d\n",client_sockfd,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

                bzero(&ev,sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(client_sockfd);
                epoll_ctl(epfd,EPOLL_CTL_ADD,client_sockfd,&ev);//将该客户端socket fd添加到epoll
            }else if(events[i].events & EPOLLIN){//发生事件的是客户端，可读时间
                char buf[READ_BUFFER];
                while(true){//由于使用非阻塞式IO，读取客户端buffer，一次读取buf大小数据，知道全部读取完毕
                    bzero(&buf,sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd,buf,sizeof(buf));
                    if(bytes_read > 0){
                        printf("message from client fd %d:%s\n",events[i].data.fd,buf);
                        write(events[i].data.fd,buf,sizeof(buf));
                    }else if(bytes_read == -1 && errno == EINTR){//客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    }else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once,error:%d\n",events[i].data.fd);
                    }else if(bytes_read == 0){//EOF,客户端断开连接
                        printf("EOF,client fd %d disconnected\n",events[i].data.fd);
                        close(events[i].data.fd);//关闭socket会自动将文件描述符从epoll树上删除
                        break;
                    }
                }
            }else{//其他时间，后续实现
                printf("something else happened\n");
            }
        }
    }
    close(sockfd);
    return 0;
}

//异常退出
void errif(bool condition,const char* errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

//将指定套接字设定为非阻塞
void setnonblocking(int fd){
    fcntl(fd,F_SETFL,fcntl(fd,F_GETFL | O_NONBLOCK));
}