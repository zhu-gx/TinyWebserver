#include<sys/socket.h>
#include<stdio.h>//perror
#include<stdlib.h>//exit
#include<arpa/inet.h>//sockaddr_in
#include<cstring>//bzero
#include<unistd.h>//read&write

void errif(bool condition,const char* errmsg);//错误处理函数

int main(){
    //IP地址采用IPv4；数据传输方式，SOCK_STREAM多用于TCP；协议
    //创建一个套接字sockfd，对外提供一个网络通信接口
    //Linux系统中，网络连接属于一个文件，用文件描述符表示
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    errif(sockfd == -1,"socket");

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
    errif(listenfd == -1,"listen");

    //监听后，需要accept接受客户端请求
    //对于每个客户端，在接受连接同时要保存客户端的socket地址信息
    //accept会阻塞当前程序，直到有一个客户端socket被接受后程序才会往下运行
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr,sizeof(client_addr));
    int client_sockfd = accept(sockfd,(sockaddr*)&client_addr,&client_addr_len);
    errif(client_sockfd == -1,"accept");
    printf("new client fd %d \n IP:%s \n Port:%d \n",client_sockfd,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

    //建立socket连接后
    //使用read和write进行网络接口的数据读写
    while(true){
        char buf[1024];//定义缓冲区
        bzero(&buf,sizeof(buf));//清空缓冲区
        ssize_t read_bytes = read(client_sockfd,buf,sizeof(buf));//从客户端socket读到缓冲区，返回已读数据大小
        if(read_bytes > 0){
            printf("message from client fd %d:%s \n",client_sockfd,buf);
            write(client_sockfd,buf,sizeof(buf));
        }else if(read_bytes == 0){//read返回0，表示EOF（已到达文件尾/无可读取的数据）
            printf("client fd %d disconnected\n",client_sockfd);
            const char *str = (close(client_sockfd)==0) ? "successful close client fd" : "fail close client fd";
            printf("%s\n",str);
            break;
        }else if(read_bytes == -1){
            const char *str = (close(client_sockfd)==0) ? "successful close client socketfd" : "fail close client socketfd";
            printf("%s\n",str);
            errif(true,"socket read error");
        }
    }
}

//异常退出
void errif(bool condition,const char* errmsg){
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
