#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/uio.h>
#include <signal.h>
#include <arpa/inet.h>

extern void sig_proccess(int signo);
extern void sig_pipe(int signo);
extern void process_conn_client(int sc);

static int s;

void sig_proccess_client(int signo){
    printf("catch a exit signal");
    close(s);
    exit(0);
}

#define PORT 8888

int main(int argc,char* argv[]){
    struct sockaddr_in server_addr;
    int err;

    if(argc == 1){
        printf("PLS input server addr\n");
        return 0;
    }

    signal(SIGINT,sig_proccess);

    signal(SIGPIPE,sig_pipe);

    s = socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        printf("socket error\n");
        return -1;
    }
    //设置服务器地址
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    connect(s,(struct sockaddr*) &server_addr,sizeof(struct sockaddr));

    process_conn_client(s);
    close(s);

}
