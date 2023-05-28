#include<sys/socket.h>
#include<stdlib.h>//exit
#include<stdio.h>//perror
#include<arpa/inet.h>//sockaddr_in
#include<cstring>//bzero
#include<unistd.h>//read&write
#include<sys/epoll.h>//epoll

#define BUFFER_SIZE 1024

void errif(bool condition,const char* errmsg);//错误处理函数

int main(){
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    errif(sockfd == -1,"socket open fail");
    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(9999);
    int connectfd = connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr));
    errif(connectfd == -1,"client connect fail");

    //使用read和wirte对网络连接进行读写
    while(true){
        char buf[BUFFER_SIZE];//定义缓冲区
        bzero(&buf,sizeof(buf));//清空缓冲区
        scanf("%s",buf);//从键盘读取数据，写入缓冲区
        ssize_t write_bytes = write(sockfd,buf,sizeof(buf));//发送缓冲区中的数据到服务器socket，返回已发送数据大小
        errif(write_bytes == -1,"socket already disconnected,can't write any more!\n");
        bzero(&buf,sizeof(buf));//清空缓冲区
        ssize_t read_bytes = read(sockfd,buf,sizeof(buf));//从服务器socket读到缓冲区，返回已读数据大小
        if(read_bytes > 0){
            printf("message from server:%s\n",buf);
        }else if(read_bytes == 0){//read返回0，表示EOF（通常是服务端断开连接）
            printf("server socket disconnected!\n");
            break;
        }else if(read_bytes == -1){//read返回-1，异常处理
            //Linux的文件描述符存在理论上限，使用完后建议用close关闭
            const char *str = (close(sockfd)==0) ? "successful close socketfd" : "fail close socketfd";
            printf("%s\n",str);
            errif(true,"socket read error");
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