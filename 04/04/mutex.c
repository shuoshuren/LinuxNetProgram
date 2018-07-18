#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

void *producter_f(void *arg);//生产者
void *consumer_f(void *arg);//消费者
int buffer_has_item = 0;
pthread_mutex_t mutex;
int running = -1;


int main(void){
    pthread_t consumer_t;
    pthread_t producter_t;

    pthread_mutex_init(&mutex,NULL);//初始化互斥
    pthread_create(&producter_t,NULL,(void*)producter_f,NULL);
    pthread_create(&consumer_t,NULL,(void *)consumer_f,NULL);

    usleep(1);
    running = 0;
    pthread_join(consumer_t,NULL);
    pthread_join(producter_t,NULL);
    pthread_mutex_destroy(&mutex);//销毁互斥

    return 0;
}

/*生产者线程程序*/
void *producter_f(void *arg){
    while(running){
        pthread_mutex_lock(&mutex);//进入互斥区
        buffer_has_item++;
        printf("生产,总数：%d\n",buffer_has_item);
        pthread_mutex_unlock(&mutex);//离开互斥区
    }
}


/*消费者线程程序*/
void *consumer_f(void *arg){
    while(running){
        pthread_mutex_lock(&mutex);//进入互斥区
        buffer_has_item--;
        printf("消费,总数：%d\n",buffer_has_item);
        pthread_mutex_unlock(&mutex);//离开互斥区
    }
}

