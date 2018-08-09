//
// Created by xc on 18-8-8.
// SIP网络协议栈的网络数据缓冲区
//

#ifndef SIP_SIP_SKBUFF_H
#define SIP_SIP_SKBUFF_H

#include <zconf.h>

/**
 * 存储缓冲结构，主要包含各个不同层次的协议结构指针，控制指针和网络数据指针
 */
struct skbuff{
    struct skbuff *next;//下一个skbuff结构
    union{
        struct sip_tcphdr *tcph;//TCP协议头指针
        struct sip_udphdr *udph;//UDP
        struct sip_icmphdr *icmph;//ICMP
        struct sip_igmphdr *igmph;//IGMP
        __uint8_t  *raw;//传输层原始数据
    } th;//传输层变量
    union{
        struct sip_iphdr *iph;//ip
        struct sip_arphdr *arph;//arp
        __uint8_t *raw;//网络层原始数据
    } nh;//网络层
    union{
        struct sip_ethhdr *ethh;//物理层以太网头部
        __uint8_t *raw;//物理层变量
    } phy;//物理层
    struct net_device *dev;//网卡设备
    __int16_t protocol;//协议类型
    __uint32_t tot_len;//skbuff中网络数据的总长度
    __uint32_t len;//skbuff当前协议层数据长度
    __uint8_t csum;//校验和
    __uint8_t ip_summed;//ip层头部是否进行了校验
    __uint8_t *head;//实际网络层数据的头部指针
    __uint8_t *data;//当前层网络数据的头部指针
    __uint8_t *tail;//当前层网络数据的尾部指针
    __uint8_t *end;//实际网络数据的尾部指针
};

/**
 * 申请用户指定大小缓存区域
 * @param size
 * @return
 */
struct skbuff * skb_alloc(__uint32_t size);

/**
 * 释放函数
 * @param skb
 */
void skb_free(struct skbuff *skb);

/**
 * 复制skbuff
 * @param from
 * @param to
 */
void skb_clone(struct skbuff *from,struct skbuff *to);

/**
 * 移动skbuff的tail指针
 * @param skb
 * @param len
 * @return
 */
__uint8_t skb_put(struct skbuff *skb,__uint32_t len);

#endif //SIP_SIP_SKBUFF_H
