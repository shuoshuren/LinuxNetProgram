#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <time.h>

void show_msg(int msg_id,struct msqid_ds msg_info){
    int ret = -1;
    sleep(1);
    ret = msgctl(msg_id,IPC_STAT,&msg_info);//获取消息
    if(-1 == ret){
        printf("获取消息失败\n");
        return;
    }
    printf("\n");
    printf("字节数:%ld\n",msg_info.msg_cbytes);
    printf("消息数:%ld\n",msg_info.msg_qnum);
    printf("最大字节数：%ld\n",msg_info.msg_qbytes);
    printf("发送消息的pid:%d\n",msg_info.msg_lspid);
    printf("接受消息的pid:%d\n",msg_info.msg_lrpid);
    printf("发送消息的时间：%s",ctime(&(msg_info.msg_stime)));
    printf("接受消息的时间：%s",ctime(&(msg_info.msg_rtime)));
    printf("最后变化的时间：%s",ctime(&(msg_info.msg_ctime)));
    printf("消息的UID：%d",msg_info.msg_perm.uid);
    printf("消息的GID：%d",msg_info.msg_perm.gid);
}


int main(void){
    int ret = -1;
    int msg_flags,msg_id;
    key_t key;
    struct msgmbuf{
        int mtype;
        char mtext[10];
    };

    struct msqid_ds msg_info;
    struct msgmbuf msg_mbuf;

    int msg_sflags,msg_rflags;
    
    char *msgpath = "/tmp";//消息key产生所用的路径
    key = ftok(msgpath,'b');//产生key
    if(key != -1){
        printf("成功建立key\n");
    }else{
        printf("建立key失败\n");
    }

    msg_flags = IPC_CREAT|IPC_EXCL;
    msg_id = msgget(key,msg_flags|0x066);//建立消息
    if(-1 == msg_id){
        printf("消息建立失败\n");
        return 0;
    }

    show_msg(msg_id,msg_info);//显示消息属性

    msg_sflags = IPC_NOWAIT;
    msg_mbuf.mtype = 10;
    memcpy(msg_mbuf.mtext,"测试消息",sizeof("测试消息"));
    ret = msgsnd(msg_id,&msg_mbuf,sizeof("测试消息"),msg_sflags);//发送消息

    if(-1 == ret){
        printf("发送消息失败\n");
    }

    show_msg(msg_id,msg_info);

    msg_rflags = IPC_NOWAIT | MSG_NOERROR;
    ret = msgrcv(msg_id,&msg_mbuf,10,10,msg_rflags);//接受消息
    if(-1 == ret){
        printf("接受消息失败\n");
    }else{
        printf("接受消息成功，长度：%d\n",ret);
    }

    show_msg(msg_id,msg_info);

    msg_info.msg_perm.uid = 8;
    msg_info.msg_perm.gid = 8;
    msg_info.msg_qbytes = 12345;
    ret = msgctl(msg_id,IPC_SET,&msg_info);//设置消息属性
    if(-1 == ret){
        printf("设置消息属性失败\n");
        return 0;
    }

    show_msg(msg_id,msg_info);

    ret = msgctl(msg_id,IPC_RMID,NULL);
    if(-1 == ret){
        printf("删除消息失败\n");
        return 0;
    }

    return 0;




}
