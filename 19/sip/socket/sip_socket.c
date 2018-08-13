//
// Created by xc on 18-8-8.
//

#include <semaphore.h>
#include <memory.h>
#include "sip_socket.h"
#include "../udp/sip_udp.h"
#include "../skbuff/sip_skbuff.h"
#include "../ip/sip_ip.h"


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

int sip_sock_connect(struct sock *sock, struct in_addr *ip, uint16_t port) {
    return 0;
}

void sip_sock_sendto(struct sock *sock, struct skbuff *skb, struct in_addr *ip, in_port_t port) {

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




/**
 * socket建立函数
 * @param domain
 * @param type
 * @param protocol
 * @return
 */
int sip_socket(int domain,int type,int protocol){
    struct sock *sock;
    int i = 0;
    if(domain != AF_INET || protocol != 0){
        return -1;
    }
    switch (type){
        case SOCK_DGRAM:
            sock = (struct sock *) sip_udp_new();
            break;
        case SOCK_STREAM:
            break;
        default:
            return -1;
    }
    if(!sock){
        return -1;
    }
    i = alloc_socket(sock);
    if(i == -1){
        sip_sock_delete(sock);
        return -1;
    }
    sock->socket = i;
    return i;
}

/**
 * 获取socket类型映射
 * @param s
 * @return
 */
struct sip_socket *get_socket(int s) {
    return NULL;
}


int alloc_socket(struct sock *sock) {
    return 0;
}

void sip_sock_delete(struct sock *sock){

}

/**
 * 套接字关闭函数
 * @param s
 * @return
 */
int sip_close(int s){
    struct sip_socket *socket;
    socket = get_socket(s);
    if(!socket){
        return -1;
    }
    sip_sock_delete(socket->sock);
    if(socket->lastdata){
        skb_free(socket->lastdata);
    }
    socket->lastdata = NULL;
    socket->sock = NULL;
    return 0;
}

/**
 * socket绑定函数
 * @param sockfd
 * @param my_addr
 * @param addrlen
 * @return
 */
int sip_bind(int sockfd,const struct sockaddr *my_addr,socklen_t addrlen){
    struct sip_socket *socket;
    struct in_addr local_addr;
    __uint16_t port_local;
    int err;
    socket = get_socket(sockfd);
    if(!socket){
        return  -1;
    }
    local_addr.s_addr = ((struct sockaddr_in *)my_addr)->sin_addr.s_addr;
    port_local = ((struct sockaddr_in *)my_addr)->sin_port;
    err = sip_sock_bind(socket->sock,&local_addr,ntohs(port_local));
    if(err != 0){
        return -1;
    }
    return 0;

}


/**
 * socket 连接函数
 * @param sockfd
 * @param serv_addr
 * @param addrlen
 * @return
 */
int sip_connect(int sockfd,const struct sockaddr *serv_addr,socklen_t addrlen){
    struct sip_socket *socket;
    int err;
    socket = get_socket(sockfd);
    if(!socket){
        return -1;
    }
    struct in_addr remote_addr;
    __uint16_t remote_port;
    remote_addr.s_addr = ((struct sockaddr_in *)serv_addr)->sin_addr.s_addr;
    remote_port = ((struct sockaddr_in *)serv_addr)->sin_port;
    err = sip_sock_connect(socket->sock,&remote_addr,ntohs(remote_port));
    return 0;
}



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
                    struct sockaddr *from,socklen_t *fromlen){
    struct sip_socket *socket;
    struct skbuff *skb;
    struct sockaddr_in *f = (struct sockaddr_in *)from;
    int len_copy = 0;
    socket = get_socket(s);
    if(!socket){
        return -1;
    }
    if(!socket->lastdata){
        socket->lastdata = (struct skbuff *)sip_sock_recv(socket->sock);
        socket->lastoffset = 0;
    }
    skb = socket->lastdata;
    *fromlen = sizeof(struct sockaddr_in);
    f->sin_family = AF_INET;
    f->sin_addr.s_addr = skb->nh.iph->saddr;
    f->sin_port = skb->th.udph->source;
    len_copy = skb->len - socket->lastoffset;
    if(len >len_copy){
        memcpy(buf,skb->data+socket->lastoffset,len_copy);
        skb_free(skb);
        socket->lastdata = NULL;
        socket->lastoffset = 0;
    }else{
        len_copy = len;
        memcpy(buf,skb+socket->lastoffset,len_copy);
        socket->lastoffset += len_copy;
    }
    return len_copy;
}
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
                   const struct sockaddr *to,socklen_t tolen){
    struct sip_socket *socket;
    struct in_addr remote_addr;
    struct sockaddr_in *to_in = (struct sockaddr_in *)to;
    int l_head = sizeof(struct sip_ethhdr) +sizeof(struct sip_iphdr)
            + sizeof(struct sip_udphdr);
    int size = l_head+len;
    struct skbuff *skb = skb_alloc(size);
    char *data = skb_put(skb,l_head);
    memcpy(data,buf,len);
    remote_addr = to_in->sin_addr;
    socket = get_socket(s);
    if(!socket){
        return -1;
    }
    sip_sock_sendto(socket->sock,skb,&remote_addr,to_in->sin_port);
    return len;
}

