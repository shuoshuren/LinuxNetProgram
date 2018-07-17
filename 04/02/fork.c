#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){

    pid_t pid;

    pid = fork();

    if(-1 == pid){
        printf("进程创建失败!\n");
        return -1;
    }else if(pid == 0){
        printf("子进程：%d,fork返回值：%d，父进程：%d\n",getpid(),pid,getppid());

    }else{
        printf("父进程：%d,fork返回值：%d\n",getpid(),pid);
    }

    return 0;
}
