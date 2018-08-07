//
// Created by xc on 18-8-3.
//

#include "worker.h"
#include "../config/config.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>

/**
 * 监听socket
 * @return
 */
 int do_listen(){
    int s,err;
    s = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_addr;
    if(s <0){
        printf("socket error\n");
        return -1;
    }
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(conf_para.ListenPort);
    //绑定
    err = bind(s,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(err<0){
        printf("bind error\n");
        return -1;
    }
    //设置监听
    err = listen(s,conf_para.InitClient);
    if(err<0){
        printf("listen error\n");
        return -1;
    }
    return s;

}

/**
 * 当有客户端连接来时，将客户端连接分配给空闲客户端，由客户端处理请求
 * @param ss
 * @return
 */
int worker_schedule_run(int ss){
    printf("==>worker schedule run\n");
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    //初始化业务线程
    worker_init();

    int i = 0;
    for(;SCHEDULE_STATUS == STATUS_RUNNING;){
        struct timeval tv;
        fd_set rfds;
        int retval = -1;
        //清空读文件集，将客户端连接描述符放入读文件集
        FD_ZERO(&rfds);
        FD_SET(ss,&rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 500000;

        retval = select(ss+1,&rfds,NULL,NULL,&tv);
        switch(retval){
            case -1:
            case 0:
                continue;
                break;
            default:
                if(FD_ISSET(ss,&rfds)){
                    int sc = accept(ss,(struct sockaddr *)&client,&len);
                    i = worker_is_status(WORKER_IDEL);//查找空闲业务线程
                    if(i == -1){
                        i = worker_is_status(WORKER_DETACHED);//是否到达最大客户端数
                        if(i != -1){
                            worker_add(i);
                        }
                    }
                    if(i != -1){
                        wctls[i].conn.cs = sc;
                        pthread_mutex_unlock(&wctls[i].opts.mutex);

                    }
                }
                break;
        }

    }
    printf("<== worker schedule run\n");
    return 0;

}

/**
 * 停止调度
 * @return
 */
int worker_schedule_stop(){
    printf("==>worker schedule stop\n");
    SCHEDULE_STATUS = STATUS_STOP;
    int i=0;
    worker_destroy();
    int allfired = 0;
    for(;!allfired;){
        allfired = 1;
        for(i =0;i<conf_para.MaxClient;i++){
            int flags = wctls[i].opts.flags;
            if(flags == WORKER_DETACHING || flags == WORKER_IDEL){
                allfired = 0;
            }
        }
    }

    pthread_mutex_destroy(&thread_init);
    for(i =0;i<conf_para.MaxClient;i++){
        pthread_mutex_destroy(&wctls[i].opts.mutex);
    }
    free(wctls);
    printf("<== worker schedule stop\n");
    return 0;
}

/**
 * 初始化线程
 */
static void worker_init(){
    printf("==> worker init\n");
    int i=0;
    wctls = (struct worker_ctl *)malloc(sizeof(struct worker_ctl) * conf_para.MaxClient);
    memset(wctls,0, sizeof(*wctls)* conf_para.MaxClient);

    for(i =0;i<conf_para.MaxClient;i++){
        //opts和conn结构与worker_ctl结构形成回指针
        wctls[i].opts.work = &wctls[i];
        wctls[i].conn.work = &wctls[i];
        //opts 初始化
        wctls[i].opts.flags = WORKER_DETACHED;
        pthread_mutex_init(&wctls[i].opts.mutex,NULL);
        pthread_mutex_lock(&wctls[i].opts.mutex);
        //conn 初始化
        wctls[i].conn.con_req.conn = &wctls[i].conn;
        wctls[i].conn.con_res.conn = &wctls[i].conn;
        wctls[i].conn.cs = -1;
        //con_req部分初始化
        wctls[i].conn.con_req.req.ptr = wctls[i].conn.dreq;
        wctls[i].conn.con_req.head = wctls[i].conn.dreq;
        wctls[i].conn.con_req.uri = wctls[i].conn.dreq;
        //con_res部分初始化
        wctls[i].conn.con_res.fd = -1;
        wctls[i].conn.con_res.res.ptr = wctls[i].conn.dres;
    }
    for(i = 0;i<conf_para.InitClient;i++){
        worker_add(i);
    }
    printf("<==worker init\n");
}

static void worker_destroy(){

}

/**
 * 查找状态相同的worker
 * @param status
 * @return -1表示找不到
 */
int worker_is_status(int status){
    for (int i = 0; i < conf_para.MaxClient; ++i) {
        if(wctls[i].opts.flags == status){
            return i;
        }
    }
    return -1;
}

/**
 *  添加业务线程
 * @param position
 * @return
 */
void worker_add(int position){
    wctls[position].opts.flags == WORKER_RUNNING;
}