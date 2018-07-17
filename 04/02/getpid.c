#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(){

    pid_t pid,ppid;
    
    //获取当前进程和父进程的id
    pid = getpid();
    ppid = getppid();

    printf("当前进程的ID号为：%d\n",pid);
    printf("当前父进程的ID号为：%d\n",ppid);
    return 0;

}
