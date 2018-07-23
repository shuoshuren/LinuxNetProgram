#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

static struct iovec *vs = NULL,*vc = NULL;

/*服务器对客户端的处理*/
void process_conn_server(int s){

    ssize_t size = 0;
    char buffer[1024];

    struct iovec*v = (struct iovec*)malloc(3*sizeof(struct iovec));
    if(!v){
        printf("Not enough memory\n");
        return;
    }

    vs = v;//挂接全局变量，便于释放管理
    v[0].iov_base = buffer;
    v[1].iov_base = buffer + 10;
    v[2].iov_base = buffer + 20;
    v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;


    for(;;){
        size = readv(s,v,3);

        if(size == 0){
            return;
        }

        //构建响应字符
        sprintf(v[0].iov_base,"%ld ",size);
        sprintf(v[1].iov_base,"bytes alt");
        sprintf(v[2].iov_base,"ogether\n");

        v[0].iov_len = strlen(v[0].iov_base);
        v[1].iov_len = strlen(v[1].iov_base);
        v[2].iov_len = strlen(v[2].iov_base);
        writev(s,v,3);//发送给客户端
    }
}

/*客户端处理过程*/
void process_conn_client(int s){
    
    ssize_t size = 0;
    char buffer[30];

    struct iovec *v = (struct iovec*)malloc(3*sizeof(struct iovec));

    if(!v){
        printf("Not enough memory\n");
        return;
    }

    vc = v;

    v[0].iov_base = buffer;
    v[1].iov_base = buffer + 10;
    v[2].iov_base = buffer + 20;
    v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;

    int i =0;

    for(;;){
        size = read(0,v[0].iov_base,10);
        if(size > 0){
            v[0].iov_len = size;
            writev(s,v,1);
            v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
            size = readv(s,v,3);
            for(i=0;i<3;i++){
                if(v[i].iov_len>0){
                    write(1,v[i].iov_base,v[i].iov_len);
                }
            }

        
        }

    }
}


/*信号处理函数*/
void sig_proccess(int signo){
    printf("catch a exit signal\n");
  
    //释放资源
    free(vc);
    free(vs);
    _exit(0);
}

/*sig pipe处理函数*/
void sig_pipe(int sign){
    printf("catch a sigpipe signal\n");
  
    //释放资源
    free(vc);
    free(vs);
    _exit(0);
    
}


