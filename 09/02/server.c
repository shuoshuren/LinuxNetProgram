#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

extern void sig_proccess(int signo);
extern void process_conn_server(int sc);
#define PORT 8888
#define BACKLOG 2

int main(int argc,char *argv[]){

    int ss,sc;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int err;
    pid_t pid;

    signal(SIGINT,sig_proccess);
    signal(SIGPIPE,sig_proccess);

    ss = socket(AF_INET,SOCK_STREAM,0);
    if(ss<0){
        printf("socket error\n");
        return -1;
    }

    //设置服务器地址
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    err = bind(ss,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if(err<0){
        printf("bind error\n");
        return -1;
    }

    err = listen(ss,BACKLOG);
    if(err <0){
        printf("listen error\n");
        return -1;
    }

    for(;;){
        int addrlen = sizeof(struct sockaddr);

        sc = accept(ss,(struct sockaddr*)&client_addr,&addrlen);
        if(sc<0){
            continue;
        }

        pid = fork();
        if(pid == 0){
            close(ss);
            process_conn_server(sc);
        }else{
            close(sc);
        }
    }

}
