//
// Created by xc on 18-8-8.
// 用户接口
//

#ifndef SIP_SIP_SOCKET_H
#define SIP_SIP_SOCKET_H

#include <bits/semaphore.h>
#include <arpa/inet.h>
#include "../skbuff/sip_skbuff.h"



struct sock {
    int type;//协议类型
    int state;//协议状态
    union {
        struct ip_pcb *ip;//ip层控制结构
        struct tcp_pcb *tcp;//tcp层控制结构
        struct udp_pcb *udp;//udp层控制结构
    } pcb;
    int err;//错误值
    struct skbuff *skb_recv;//接收缓冲区
    sem_t *sem_recv;//接受缓冲区计数信号量
    int socket;//socket对应的文件描述符
    int recv_timeout;//接收数据超时时间
    __uint16_t recv_avail;//可以接收数据
};

/**
 * bind
 * @param sock
 * @param addr
 * @param port
 * @return
 */
int sip_sock_bind(struct sock *sock,struct in_addr *addr,__uint16_t port);

/**
 * 接收函数
 * @param sock
 * @return
 */
struct skbuff * sip_sock_recv(struct sock *sock);

#endif //SIP_SIP_SOCKET_H
