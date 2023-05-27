#include<sys/socket.h>
#include<stdlib.h>//exit
#include<stdio.h>//perror
#include<arpa/inet.h>//sockaddr_in
#include<cstring>//bzero

int main(){
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1){
        perror("socket");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    bzero(&serv_addr,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(9999);
    connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr));
}