#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <semaphore.h>

void *producter_f(void *arg);//生产者
void *consumer_f(void *arg);//消费者
int running = 1;
sem_t sem;


int main(void){
    pthread_t consumer_t;
    pthread_t producter_t;

    sem_init(&sem,0,16);//信号量初始化

    pthread_create(&producter_t,NULL,(void*)producter_f,NULL);
    pthread_create(&consumer_t,NULL,(void *)consumer_f,NULL);

    usleep(1);
    running = 0;
    pthread_join(consumer_t,NULL);
    pthread_join(producter_t,NULL);

    sem_destroy(&sem);//销毁信号量

    return 0;
}

/*生产者线程程序*/
void *producter_f(void *arg){
    int semval = 0;
    while(running){
        usleep(1);
        sem_post(&sem);//信号量增加
        sem_getvalue(&sem,&semval);//获取信号量的值
        printf("生产,总数：%d\n",semval);
    }
}


/*消费者线程程序*/
void *consumer_f(void *arg){
    int semval = 0;
    while(running){
        usleep(1);
        sem_wait(&sem);//等待信号量
        sem_getvalue(&sem,&semval);//获取信号量的值
        printf("消费,总数：%d\n",semval);
    }
}

