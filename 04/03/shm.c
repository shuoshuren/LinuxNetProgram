#include <stdio.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

int main(void){
    
    char *msg = "你好，共享内存\n";

    int shmid;
    char *shms,*shmc;
    int value = 0;
    char buffer[80];
    pid_t p;

    shmid = shmget(IPC_PRIVATE,1024,IPC_CREAT|0604);

    p = fork();

    if(p>0){//父进程
        shms = shmat(shmid,0,0);//挂载共享内存

        memcpy(shms,msg,strlen(msg)+1);//复制内容

        sleep(5);

        shmdt(shms);//摘除共享内存
    }else if(p == 0){//子进程
    
        shmc = shmat(shmid,0,0);//挂载共享内存
        printf("共享内存的值：%s\n",shmc);
        shmdt(shmc);//摘除共享内存
    }

    return 0;
}
