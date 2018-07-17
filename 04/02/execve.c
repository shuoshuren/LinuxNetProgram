#include <stdio.h>
#include <unistd.h>

int main(void){
    char *args[] = {"/bin/ls",NULL};

    printf("系统进程号：%d\n",getpid());
    if(execve("/bin/ls",args,NULL)<0){
        printf("创建进程出错！\n");
    }

    return 0;
}
