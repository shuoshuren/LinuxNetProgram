#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

/* 控制文件符*/
int main(void){
    
    int flags = -1;
    int accmode = -1;

    flags = fcntl(0,F_GETFL,0);//获取标准输入的状态
    if(flags < 0){
        printf("failure to use fcntl\n");
        return -1;
    }

    accmode = flags & O_ACCMODE;//获得访问模式
    switch(accmode){
        case O_RDONLY:
            printf("只读\n");
            break;
        case O_WRONLY:
            printf("只写\n");
            break;
        case O_RDWR:
            printf("可读写\n");
            break;
        default:
            printf("其他模式\n");
            break;
    }

    if(flags & O_APPEND){
        printf("附加模式");
    }

    if(flags & O_NONBLOCK){
        printf("非阻塞模式");
    }
    
    return 0;
}
