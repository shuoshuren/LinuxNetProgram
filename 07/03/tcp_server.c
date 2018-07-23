#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/in.h>

#define PORT 8888
#define BACKLOG 2

void process_conn_server(int sc);

int main(int argc,char *argv[]){
    int ss,sc;//ss 为服务器的socket描述符，sc为客户端socket描述符
    struct sockaddr_in server_addr;//服务器地址
    struct sockaddr_in client_addr;//客户端地址
    int err;
    pid_t pid;
    //建立流式套接字
    ss = socket(AF_INET,SOCK_STREAM,0);
    if(ss <0){
        printf("socket error\n");
        return -1;
    }
    //设置服务器地址
    bzero(&server_addr,sizeof(server_addr));//清零
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    //绑定地址到套接字描述符
    err = bind(ss,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(err <0){
        printf("bind error\n");
        return -1;
    }
    //设置监听
    err = listen(ss,BACKLOG);
    if(err <0){
        printf("listen error\n");
        return -1;
    }
    //主循环
    for(;;){
        int addrlen = sizeof(struct sockaddr);

        sc = accept(ss,(struct sockaddr*)&client_addr,&addrlen);
        //接受客户端连接
        if(sc <0){
            continue;
        }

        pid = fork();
        if(pid == 0){
            close(ss);//在子进程中关闭服务器监听
            process_conn_server(sc);
        }else{
            close(sc);//在父进程中关闭客户端连接
        }

    }

}

/* 服务器对客户端的处理*/
void process_conn_server(int s){

    ssize_t size = 0;
    char buffer[1024];

    for(;;){
        size = read(s,buffer,1024);//从socket中读取数据到缓冲区
        if(size == 0){
            return;
        }
        //构建响应字符
        sprintf(buffer,"%ld bytes altogether\n",size);
        write(s,buffer,strlen(buffer)+1);//发送给客户端

    }
}

