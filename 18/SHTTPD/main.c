#include <stdio.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include "worker/worker.h"
#include "config/config.h"

/**
 * 信号截取函数
 * @param num
 */
static void sig_int(int num);

int main(int argc,char *argv[]) {
    signal(SIGINT,sig_int);
    para_init(argc,argv);
    int s = do_listen();
    worker_schedule_run(s);
    return 0;
}

/**
 * 信号截取函数
 * @param num
 */
static void sig_int(int num){
    worker_schedule_stop();

    return;
}