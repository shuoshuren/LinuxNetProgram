#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static int run = 1;
static int retvalue;

/* 线程处理函数*/
void * start_routine(void *arg){
    int *running = arg;
    printf("子线程传入参数：%d\n",*running);
    while(*running){
        printf("子线程正在运行\n");
        usleep(1);
    }
    printf("子线程退出");
    retvalue = 8;
    pthread_exit((void *) &retvalue);//线程退出并设置退出值
}

int main(void){
    pthread_t pt;
    int ret = -1;
    int times = 3;
    int i = 0;

    int *ret_join = NULL;

    //建立线程
    ret = pthread_create(&pt,NULL,(void *)start_routine,&run);
    
    if(ret != 0){
        printf("建立线程失败\n");
        return 1;
    }

    usleep(1);

    for(;i<times;i++){
        printf("主线程打印\n");
        usleep(1);
    }

    run = 0;
    //等待线程退出
    pthread_join(pt,(void*)&ret_join);

    printf("线程返回值为:%d\n",*ret_join);
    return 0;


}
