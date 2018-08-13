//
// Created by xc on 18-8-8.
//

#include <semaphore.h>
#include "sip_socket.h"
#include "../udp/sip_udp.h"


/**
 * bind
 * @param sock
 * @param addr
 * @param port
 * @return
 */
int sip_sock_bind(struct sock *sock,struct in_addr *addr,__uint16_t port){
    if(sock->pcb.tcp != NULL){
        switch (sock->type){
            case SOCK_RAW:
                break;
            case SOCK_DGRAM:
                sock->err = sip_udp_bind(sock->pcb.udp,addr,port);
                break;
            case SOCK_STREAM:
                break;
            default:
                break;
        }
    }
}

/**
 * 接收函数
 * @param sock
 * @return
 */
struct skbuff * sip_sock_recv(struct sock *sock){
    struct skbuff *skb_recv = NULL;
    int num = 0;
    if(sem_getvalue(&sock->sem_recv,&num)){//没有获取到网络数据，超时等待
        struct timespec timeout;
        timeout.tv_sec = sock->recv_timeout;
        timeout.tv_nsec = 0;
        sem_timedwait(&sock->sem_recv,&timeout);
    }else{//已有数据，直接获取数据
        sem_wait(&sock->sem_recv);
    }
    skb_recv = sock->skb_recv;
    if(skb_recv == NULL){
        return NULL;
    }
    sock->skb_recv = skb_recv->next;
    skb_recv->next = NULL;
    return skb_recv;
}