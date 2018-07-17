#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(void){


    int result = -1;
    int fd[2],nbytes;
    pid_t pid;

    int *read_fd = &fd[0];
    int *write_fd = &fd[1];
    char string[] = "你好，管道";
    char readbuffer[80];

    result = pipe(fd);

    if(-1 == result){
        printf("建立管道失败\n");
        return -1;
    }

    pid = fork();
    if(-1 == pid){
        printf("fork 失败\n");
        return -1;
    }else if(0 == pid){//子进程
        close(*read_fd);
        
        result = write(*write_fd,string,strlen(string));

        return 0;
    }else {//父进程
        close(*write_fd);
        nbytes = read(*read_fd,readbuffer,sizeof(readbuffer));

        printf("接收到%d个数据，内容：%s\n",nbytes,readbuffer);
    
    }
    return 0;

}
