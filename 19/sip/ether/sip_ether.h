//
// Created by xc on 18-8-8.
// SIP 网络接口层
//

#ifndef SIP_SIP_ETHER_H
#define SIP_SIP_ETHER_H

#include <zconf.h>
#include <arpa/inet.h>
#include "../skbuff/sip_skbuff.h"

#define ETH_P_IP 0x0800 /*ip类型的报文*/
#define ETH_P_ARP 0x0806    //ARP报文
#define ETH_LEN 0
#define ETH_ALEN 6  //以太网地址长度
#define ETH_HLEN 14 //以太网头部长度
#define ETH_ZLEN 60     //以太网最小长度
#define ETH_DEAT_LEN 1500   //以太网最大负载长度
#define ETH_FRAM_LEN 1514   //以太网最大长度
#define ETH_P_ALL 0x0003    //使用SOCk_PACKET获取每一个包
#define ETH_P_802_3 0x8023

#define IFNAMSIZ 2048

/**
 * 网络设备结构
 */
struct net_device{
    char name[IFNAMSIZ];//网卡名称
    struct in_addr ip_host;//本机ip
    struct in_addr ip_netmask;//本机子网掩码
    struct in_addr ip_broadcast;//本机广播地址
    struct in_addr ip_gw;//本机网管
    struct in_addr ip_dest;//目的IP
    __uint16_t type;//以太网协议类型
    /**
     * 用于从网络设备中获取数据传入网络协议栈进行处理
     * @param skb
     * @param dev
     * @return
     */
    __uint8_t (* input)(struct skbuff *skb,struct net_device *dev);

    /**
     * 用于IP模块发送数据时候调用，会先调用ARP模块对IP地址进行查找，然后发送数据
     * @param skb
     * @param dev
     * @return
     */
    __uint8_t (* output)(struct skbuff *skb,struct net_device *dev);

    /**
     * 由ARP模块调用，直接发送网络数据
     * @param skb
     * @param dev
     * @return
     */
    __uint8_t (* linkoutput)(struct skbuff *skb,struct net_device *dev);

    __uint8_t hwaddr_len;//硬件地址长度
    __uint8_t hwaddr[ETH_ALEN];//硬件地址的值，MAC
    __uint8_t hwbroadcast[ETH_ALEN];//硬件的广播地址
    __uint8_t mtu;//网卡最大传输长度
    int s;//SOCK_PACKET建立的套接字描述符
    struct sockaddr to;//SOCK_PACKET发送的目的地址
};

struct sip_ethhdr{
    __uint8_t h_dest[ETH_ALEN];//目的以太网址
    __uint8_t h_source[ETH_ALEN];//源以太网地址
    __uint16_t h_proto;//数据包类型
};

static void sip_init_ethnet(struct net_device *dev);

/**
     * 用于从网络设备中获取数据传入网络协议栈进行处理
     * @param skb
     * @param dev
     * @return
     */
static __uint8_t * input(struct skbuff *pskb,struct net_device *dev);

/**
 * 用于IP模块发送数据时候调用，会先调用ARP模块对IP地址进行查找，然后发送数据
 * @param skb
 * @param dev
 * @return
 */
static __uint8_t * output(struct skbuff *skb,struct net_device *dev);

/**
 * 由ARP模块调用，直接发送网络数据
 * @param skb
 * @param dev
 * @return
 */
static __uint8_t * lowoutput(struct skbuff *skb,struct net_device *dev);


/**
 * 判断是否是相同的mac
 */
int samemac(__uint8_t *mac1,__uint8_t *mac2);


#endif //SIP_SIP_ETHER_H
