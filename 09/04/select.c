#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(void){
    fd_set rd;//读文件集合
    struct timeval tv;//时间间隔
    int err;

    FD_ZERO(&rd);
    FD_SET(0,&rd);//监视标准输入释放可以读数据

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    err = select(1,&rd,NULL,NULL,&tv);

    if(err == -1){
        perror("select()");
    }else if(err){
        printf("Data is available now.\n");
    }else{
    
        printf("no data within five seconds.\n");
    }

    return 0;
}
