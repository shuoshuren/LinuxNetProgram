//
// Created by xc on 18-8-8.
// IP协议
//

#ifndef SIP_SIP_IP_H
#define SIP_SIP_IP_H

#include <zconf.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../ether/sip_ether.h"
#include "../skbuff/sip_skbuff.h"

#define IPHDR_LEN 20
#define CHECKSUM_HW 1

#define IP_ADDR_ANY_VALUE 0x00000000UL
#define IP_ADDR_BROADCAST_VALUE 0xFFFFFFFFUL
#define IPREASS_TIMEOUT 0x00

#define IP_FREE_REASS(ipr)                      \
do{                                             \
    struct skbuff *skb = NULL,*skb_prev = NULL;\
    for(skb_prev = skb = ipr->skb;          \
        skb != NULL;                        \
        skb_prev = skb,                     \
        skb = skb->next,                    \
        skb_free(skb_prev));               \
        free(ipr);                          \
}while(0);

/**
 * IP 头部数据结构
 */
struct sip_iphdr{
#if defined(__LITTLE_ENDIAN)
    __uint8_t ihl:4,
     version:4;
#elif defined(__BIG_ENDIAN)
    __uint8_t version:4,
    ihl:4;
#else
#error "please fix <asm/byteorder.h>"
#endif
    __uint8_t tos;
    __uint16_t tot_len;
    __uint16_t id;
    __uint16_t frag_off;
    __uint8_t ttl;
    __uint8_t protocol;
    __uint16_t check;
    __uint32_t saddr;
    __uint32_t daddr;

};

/**
 * ip 分片重组的数据结构
 */
struct sip_reass{
    struct sip_reass *next;//下一个重组指针
    struct skbuff *skb;//分片的头指针
    struct sip_iphdr iphdr;//IP头部指针
    __uint16_t datagram_len;//数据报文的长度
    __uint8_t flags;//重组的状态
    __uint8_t timer;//时间戳
};

struct sip_reass * ip_reass_list;

/**
 * ip层输入处理
 * @param dev
 * @param skb
 * @return
 */
int ip_input(struct net_device *dev, struct skbuff *skb);

/**
 * ip层输出函数
 * @param dev
 * @param skb
 * @param src
 * @param dest
 * @param ttl
 * @param tos
 * @param proto
 * @return
 */
int ip_output(struct net_device *dev,struct skbuff *skb,struct in_addr *src,
        struct in_addr *dest,__uint8_t ttl,__uint8_t tos,__uint8_t proto);

/**
 * 分片
 * @param dev
 * @param skb
 * @return
 */
struct skbuff *ip_frag(struct net_device *dev, struct skbuff *skb);

/**
 * 计算校验和
 * @param raw
 * @param len
 * @return
 */
bool sip_chksum(__uint8_t *raw, int len);

/**
 * IP是否是广播地址
 * @param dev
 * @param ip
 * @return
 */
bool ip_is_broadcast(struct net_device *dev, __uint32_t ip);

/**
 * 重组
 * @param skb
 * @return
 */
struct skbuff *sip_reassemble(struct skbuff *skb);

#endif //SIP_SIP_IP_H
