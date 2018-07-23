#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

/*服务器对客户端的处理*/
void process_conn_server(int s){

    ssize_t size = 0;
    char buffer[1024];

    for(;;){
        size = recv(s,buffer,1024,0);

        if(size == 0){
            return;
        }

        sprintf(buffer,"%ld bytes altogether\n",size);
        send(s,buffer,strlen(buffer)+1,0);
    }
}

/*客户端处理过程*/
void process_conn_client(int s){
    
    ssize_t size = 0;
    char buffer[1024];

    for(;;){
        size = read(0,buffer,1024);

        if(size > 0){
            send(s,buffer,size,0);
            size = recv(s,buffer,1024,0);
            write(1,buffer,size);
        
        }

    }
}


/*信号处理函数*/
void sig_proccess(int signo){
    printf("catch a exit signal\n");
    _exit(0);
}

/*sig pipe处理函数*/
void sig_pipe(int sign){
    printf("catch a sigpipe signal\n");
}


