//
// Created by xc on 18-8-3.
//



#ifndef SHTTPD_WORKER_H
#define SHTTPD_WORKER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "../base/base.h"

#define STATUS_RUNNING 1
#define STATUS_STOP 2
#define WORKER_IDEL -1
#define WORKER_DETACHED 1
#define WORKER_DETACHING 2
#define WORKER_RUNNING 3

static int SCHEDULE_STATUS;

static pthread_mutex_t thread_init;

extern struct worker_ctl* wctls;

/**
 * 套接字初始化
 * @return
 */
extern int do_listen();

/**
 * 当有客户端连接来时，将客户端连接分配给空闲客户端，由客户端处理请求
 * @param ss
 * @return
 */
int worker_schedule_run(int ss);

/**
 * 停止调度
 * @return
 */
int worker_schedule_stop();

/**
 * 初始化线程
 */
static void worker_init();

/**
 * 销毁
 */
static void worker_destroy();

/**
 * 查找状态相同的worker
 * @param status
 * @return -1表示找不到
 */
int worker_is_status(int status);

/**
 *  添加业务线程
 * @param position
 * @return
 */
void worker_add(int position);

#endif //SHTTPD_WORKER_H
