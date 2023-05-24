// TCP 通信的服务器端

#include <stdio.h>
#include <arpa/inet.h>//socket头文件
#include <unistd.h>//read头文件
#include <string.h>
#include <stdlib.h>

int main() {

    // 1.创建socket(用于监听的套接字)
    //AF_INET，协议族；SOCK_STREAM，流式协议；0，TCP协议
    //返回文件描述符
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    if(lfd == -1) {
        perror("socket");
        exit(-1);
    }

    // 2.绑定
    struct sockaddr_in saddr;
    //saddr成员初始化
    saddr.sin_family = AF_INET;
    // inet_pton(AF_INET, "172.23.213.56", saddr.sin_addr.s_addr);
    saddr.sin_addr.s_addr = INADDR_ANY;  
    // INADDR_ANY，0.0.0.0任意地址，计算机的每个网卡可能不一样，表示哪一个网卡都可以绑定（仅服务端绑定可以如此）
    saddr.sin_port = htons(9999);//htons转换为网络字节序
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 3.监听
    //有无客户端连接
    ret = listen(lfd, 8);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }

    // 4.接收客户端连接
    struct sockaddr_in clientaddr;
    int len = sizeof(clientaddr);
    //lfd，监听的文件描述符；&clientaddr客户端的地址信息；&len，长度地址
    int cfd = accept(lfd, (struct sockaddr *)&clientaddr, &len);
    
    if(cfd == -1) {
        perror("accept");
        exit(-1);
    }

    // 输出客户端的信息
    //已确定有客户端连接
    char clientIP[16];
    //sin_addr，IP地址；s_addr，整数
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    //sin_port，获取端口
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    printf("client ip is %s, port is %d\n", clientIP, clientPort);

    // 5.通信
    char recvBuf[1024] = {0};
    while(1) {
        
        // 获取客户端的数据
        //从cfd中读取数据，存放在recvBuf中，sizeof(recvBuf)为大小
        int num = read(cfd, recvBuf, sizeof(recvBuf));
        if(num == -1) {
            perror("read");
            exit(-1);
        } else if(num > 0) {
            printf("recv client data : %s\n", recvBuf);
        } else if(num == 0) {
            // 表示客户端断开连接
            printf("clinet closed...");
            break;
        }

        char * data = "hello,i am server";
        // 给客户端发送数据
        write(cfd, data, strlen(data));
    }
   
    // 关闭文件描述符
    close(cfd);
    close(lfd);

    return 0;
}