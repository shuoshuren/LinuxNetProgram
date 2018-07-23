#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/in.h>

#define PORT 8888

void process_conn_client(int sc);

int main(int argc,char *argv[]){
    int s;//ss 为服务器的socket描述符，sc为客户端socket描述符
    struct sockaddr_in server_addr;//服务器地址
    int err;
    pid_t pid;
    //建立流式套接字
    s = socket(AF_INET,SOCK_STREAM,0);
    if(s <0){
        printf("socket error\n");
        return -1;
    }
    //设置服务器地址
    bzero(&server_addr,sizeof(server_addr));//清零
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    
    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);
    
    //连接服务器
    connect(s,(struct sockaddr*)&server_addr,sizeof(struct sockaddr));
    process_conn_client(s);
    close(s);
}

/* 客户端的处理过程*/
void process_conn_client(int s){

    ssize_t size = 0;
    char buffer[1024];

    for(;;){
        size = read(0,buffer,1024);//从标准输入中读取数据到缓冲区

        if(size >0){
            write(s,buffer,strlen(buffer)+1);
            size = read(s,buffer,1024);
            write(1,buffer,strlen(buffer)+1);//写到标准输出
        }

    }
}

