//
// Created by xc on 18-8-8.
//

#include <semaphore.h>
#include <memory.h>
#include "sip_udp.h"
#include "../ip/sip_ip.h"
#include "../skbuff/sip_skbuff.h"


/**
 * 端口分配方法
 * @return
 */
static __uint16_t found_a_port(){
    static __uint32_t index = 1024;
    index ++;
    return(__uint16_t) (index &0xFFFF);
}

/**
 * udp输入函数
 * @param dev
 * @param skb
 */
int sip_udp_input(struct net_device *dev, struct skbuff *skb){
    __uint16_t port = ntohs(skb->th.udph->dest);
    struct udp_pcb *upcb = NULL;
    for(upcb = udp_pcbs[port %UDP_HTABLE_SIZE];upcb != NULL;upcb = upcb->next){
        if(upcb->port_local = port){//找到
            break;
        }
    }
    if(!upcb){
        return 0;
    }
    struct sock *sock = upcb->sock;
    if(!sock){
        return 1;
    }
    struct skbuff *recv = sock->skb_recv;
    if(!recv){
        sock->skb_recv = skb;
        skb->next = NULL;
    }else{
        for(;recv->next != NULL;upcb = upcb->next);//到尾部
        recv->next = skb;
        skb->next = NULL;

    }
    return sem_post(&sock->sem_recv);//接受缓冲区计数值增加

}

/**
 * 输出函数
 * @param dev
 * @param skb
 * @param pcb
 * @param src
 * @param dest
 * @return
 */
int sip_udp_send_output(struct net_device *dev, struct skbuff *skb,
                        struct udp_pcb *pcb,struct in_addr *src,struct in_addr *dest){
    return ip_output(dev,skb,src,dest,pcb->ttl,pcb->tos,IPPROTO_UDP);
}

/**
 * 数据报文层建立函数
 * @return
 */
struct udp_pcb * sip_udp_new(void){
    struct udp_pcb *pcb = NULL;
    pcb = (struct udp_pcb *)malloc(sizeof(struct udp_pcb));
    if(pcb != NULL){
        memset(pcb,0,sizeof(struct udp_pcb));
        pcb.ttl = 255;
    }
    return pcb;
}


/**
 * 数据报文层释放函数
 */
void sip_udp_remove(struct udp_pcb *pcb){
    struct udp_pcb *pcb_t;
    int i=0;
    if(!pcb){
        return;
    }
    pcb_t = udp_pcbs[pcb->port_local%UDP_HTABLE_SIZE];
    if(!pcb_t){
        ;
    }else if(pcb_t == pcb){
        udp_pcbs[pcb->port_local%UDP_HTABLE_SIZE] = pcb_t->next;
    }else{
        for(;pcb_t->next != NULL;pcb_t = pcb_t->next){
            if(pcb_t->next == pcb){
                pcb_t->next = pcb->next;
            }
        }
    }
    free(pcb);
}

/**
 * udp 绑定函数
 * @param pcb
 * @param ipaddr
 * @param port
 * @return
 */
int sip_udp_bind(struct udp_pcb *pcb,struct in_addr *ipaddr,__uint16_t port){
    struct udp_pcb *ipcb;
    __uint8_t rebind;
    rebind = 0;
    for(ipcb = udp_pcbs[port &(UDP_HTABLE_SIZE -1)];ipcb != NULL;ipcb = ipcb->next){
        if(pcb == ipcb){//已经存在绑定
            rebind = 1;
        }
    }
    pcb->ip_local.s_addr = ipaddr->s_addr;
    if(port == 0){
#define UDP_PORT_RANGE_STAART 4096
#define UDP_PORT_RANGE_END 0x7FFF
        port  = found_a_port();
        ipcb = udp_pcbs[port];
        while ((ipcb != NULL)&&(port != UDP_PORT_RANGE_END)){
            if(ipcb->port_local == port){//已使用次端口
                port = found_a_port();
                ipcb = udp_pcbs[port];
            }else{
                ipcb = ipcb->next;
            }
        }
        if(ipcb != NULL){
            return -1;
        }
    }
    pcb->port_local = port;
    if(rebind == 0){//如果没有将次控制单元加入链表
        pcb->next = udp_pcbs[port];
        udp_pcbs[port] = pcb;
    }
    return 0;

}

/**
 * 发送数据函数
 * @param dev
 * @param pcb
 * @param skb
 * @param dst_ip
 * @param dst_port
 * @return
 */
int sip_udp_sendto(struct net_device *dev,struct udp_pcb *pcb,struct skbuff *skb,
                   struct in_addr *dst_ip,__uint16_t dst_port){
    struct sip_udphdr *udphdr;
    struct in_addr *src_ip;
    int err;
    if(pcb->port_local == 0){//没有绑定端口
        err = sip_udp_bind(pcb,&pcb->ip_local,pcb->port_local);
        if(err != 0){
            return err;
        }
    }
    udphdr = skb->th.udph;
    udphdr->source = htons(pcb->port_local);
    udphdr->dest = htons(dst_port);
    udphdr->check = 0x0000;
    if(pcb->ip_local.s_addr == 0){//本地IP
        src_ip = &dev->ip_host;
    }else{
        src_ip = &(pcb->ip_local);
    }
    udphdr->len = htons(skb->len);
    if((pcb->flags & UDP_FLAGS_NOCHKSUM) == 0){
        udphdr->check = sip_chksum_pseudo(skb,src_ip,dst_ip,IPPROTO_UDP,skb->len);
        if(udphdr->check = 0x0000){
            udphdr->check = 0xFFFF;
        }
    }
    err = sip_udp_send_output(dev,skb,pcb,src_ip,dst_ip);
    return err;
}

/**
 * udp 校验报文
 * @param skb
 * @param src
 * @param dest
 * @param proto
 * @param proto_len
 * @return
 */
__uint16_t sip_chksum_pseudo(struct skbuff *skb, struct in_addr *src,
                             struct in_addr *dest, __uint8_t proto, __uint16_t proto_len){

    __uint32_t acc;
    __uint8_t swapped;
    acc = 0;
    swapped = 0;
    {
        acc += sip_chksum(skb->data,skb->end-skb->data);
        while ((acc>>16)!= 0){
            acc = (acc&0xFFFUL) + (acc>>16);
        }
        if(skb->len%2 != 0){
            swapped = 1-swapped;
            acc = ((acc &0xff)<<8) | ((acc &0xFF00UL)>>8);
        }
    }
    if(swapped){
        acc = ((acc&0xFF)<<8) | ((acc & 0xFF00UL) >> 8);
    }
    acc += (src->s_addr& 0xFFFFUL);
    acc += ((src->s_addr>>16) & 0xFFFFUL);
    acc+= (dest->s_addr &0xFFFFUL);
    acc += ((dest->s_addr>>16)&0xFFFFUL);
    acc+= (__uint32_t)htons((__uint16_t)proto);
    acc += (__uint32_t)htons(proto_len);
    while((acc>>16) != 0){
        acc = (acc & 0xFFFFUL) + (acc >> 16);
    }
    return (__uint16_t)~(acc &0xFFFFUL);

}