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

struct sip_socket{
    struct sock *sock;//协议无关层结构指针
    struct skbuff *lastdata;//最后接收的网络数据
    __uint16_t lastoffset;//接收到网络数据的偏移量
    int err;//错误值
};

/**
 * bind
 * @param sock
 * @param addr
 * @param port
 * @return
 */
int sip_sock_bind(struct sock *sock,struct in_addr *addr,__uint16_t port);


int sip_sock_connect(struct sock *sock, struct in_addr *ip, uint16_t port);


void sip_sock_sendto(struct sock *sock, struct skbuff *skb, struct in_addr *ip, in_port_t port);

/**
 * 接收函数
 * @param sock
 * @return
 */
struct skbuff * sip_sock_recv(struct sock *sock);

/**
 * socket建立函数
 * @param domain
 * @param type
 * @param protocol
 * @return
 */
int sip_socket(int domain,int type,int protocol);

/**
 * 套接字关闭函数
 * @param s
 * @return
 */
int sip_close(int s);

/**
 * socket绑定函数
 * @param sockfd
 * @param my_addr
 * @param addrlen
 * @return
 */
int sip_bind(int sockfd,const struct sockaddr *my_addr,socklen_t addrlen);

/**
 * socket 连接函数
 * @param sockfd
 * @param serv_addr
 * @param addrlen
 * @return
 */
int sip_connect(int sockfd,const struct sockaddr *serv_addr,socklen_t addrlen);

/**
 * socket 接收数据函数
 * @param s
 * @param buf
 * @param len
 * @param flags
 * @param from
 * @param fromlen
 * @return
 */
size_t sip_recvfrom(int s,void *buf,size_t len,int flags,
        struct sockaddr *from,socklen_t *fromlen);
/**
 * socket 发送数据的函数
 * @param s
 * @param buf
 * @param len
 * @param flags
 * @param to
 * @param tolen
 * @return
 */
ssize_t sip_sendto(int s,const void *buf,size_t len,int flags,
        const struct sockaddr *to,socklen_t tolen);

int alloc_socket(struct sock *sock);

void sip_sock_delete(struct sock *sock);

struct sip_socket *get_socket(int s);

#endif //SIP_SIP_SOCKET_H
