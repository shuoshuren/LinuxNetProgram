#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>

typedef int sem_t;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    
} arg;

/*建立信号量*/
sem_t create_sem(key_t key,int value){
    union semun sem;
    sem_t semid;
    sem.val = value;

    semid = semget(key,1,IPC_CREAT|0666);
    if(-1 == semid){
        printf("create semaphore error\n");
        return -1;
    }

    semctl(semid,0,SETVAL,sem);

    return semid;
}

/* 增加信号量*/
int sem_p(sem_t semid){
    struct sembuf sops = {0,+1,IPC_NOWAIT};
    
    return (semop(semid,&sops,1));
}

/*减小信号量*/
int sem_v(sem_t semid){
    struct sembuf sops = {0,-1,IPC_NOWAIT};

    return (semop(semid,&sops,1));
}

/*设置信号量的值*/
void set_sem_value(sem_t semid,int value){
    union semun sem;
    sem.val = value;

    semctl(semid,0,SETVAL,sem);
}

/*获取信号量的值*/
int get_sem_value(sem_t semid){
    union semun sem;
    return semctl(semid,0,GETVAL,sem);
}

/*销毁信号量*/
void destroy_sem(sem_t semid){

    union semun sem;
    sem.val = 0;

    semctl(semid,0,IPC_RMID,sem);
}

int main(void){
    key_t key;
    int semid;
    char i;
    struct semid_ds buf;
    int value;

    key = ftok("/tmp",'a');

    semid = create_sem(key,100);
    
    for(i = 0;i<=3;i++){
        sem_p(semid);
        sem_v(semid);
    }

    value = get_sem_value(semid);

    printf("信号量的值为:%d\n",value);

    destroy_sem(semid);
    return 0;
}
