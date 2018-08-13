//
// Created by xc on 18-8-8.
// ICMP协议
//

#ifndef SIP_SIP_ICMP_H
#define SIP_SIP_ICMP_H

#define NR_ICMP_TYPES 18

#define ICMP_ECHOREPLY 0

#define ICMP_DEST_UNREACH 3

#define ICMP_SOURCE_QUENCH 4

#define ICMP_REDIRECT 5

#define ICMP_ECHO 8

#define ICMP_TIME_EXCEEDED 11

#define ICMP_PARAMETERPROB 12

#define ICMP_TIMESTAMP 13

#define ICMP_TIMESTAMPREPLY 14

#define ICMP_INFO_REQUEST 15

#define ICMP_INFO_REPLY 16

#define ICMP_ADDRESS 17

#define ICMP_ADDRESS_REPLY 18

#include "../skbuff/sip_skbuff.h"
#include "../ether/sip_ether.h"

struct sip_icmphdr{
    __uint8_t type;//ICMP协议类型
    __uint8_t code;//类型代码
    __uint16_t checksum;//数据校验和
    union{
        struct{
            __uint16_t id;//ID标识
            __uint16_t sequence; //数据的序列号
        } echo;//回显数据
        __uint32_t gateway;//网关
        struct{
            __uint16_t __unused;
            __uint16_t mtu;//MTU
        } frag;//分片
    }un;
};

struct icmp_control{
    void (*handler) (struct net_device *dev,struct skbuff *skb);//处理函数
    short error;//错误方式
};

/**
 * 输入函数
 * @param dev
 * @param skb
 */
static void icmp_input(struct net_device *dev, struct skbuff *skb);

void icmp_discard(struct net_device *dev,struct skbuff *skb);

void icmp_unreach(struct net_device *dev,struct skbuff *skb);

void icmp_redirect(struct net_device *dev,struct skbuff *skb);

void icmp_timestamp(struct net_device *dev,struct skbuff *skb);

void icmp_address(struct net_device *dev,struct skbuff *skb);

void icmp_address_reply(struct net_device *dev,struct skbuff *skb);

/**
 * 回显应答
 * @param dev
 * @param skb
 */
static void icmp_echo(struct net_device *dev,struct skbuff *skb);

static const struct icmp_control icmp_pointers[NR_ICMP_TYPES +1] = {
        [ICMP_ECHOREPLY] = {
                .handler = icmp_discard,
        },
        [1] = {
                .handler = icmp_discard,
                .error = 1
        },
        [2] = {
                .handler = icmp_discard,
                .error = 1
        },
        [ICMP_DEST_UNREACH] = {//主机不可达
                .handler = icmp_unreach,
                .error = 1
        },
        [ICMP_SOURCE_QUENCH] = {//源队列
                .handler = icmp_unreach,
                .error = 1
        },
        [ICMP_REDIRECT] = {//重定向
                .handler = icmp_redirect,
                .error = 1
        },
        [6] = {
                .handler = icmp_discard,
                .error = 1
        },
        [7] = {
                .handler = icmp_discard,
                .error = 1
        },
        [ICMP_ECHO] = {//回显应答
                .handler = icmp_echo,
                .error = 1
        },
        [9] = {
                .handler = icmp_discard,
                .error = 1
        },
        [10] = {
                .handler = icmp_discard,
                .error = 1
        },
        [ICMP_TIME_EXCEEDED] = {//时间超时
                .handler = icmp_unreach,
                .error = 1
        },
        [ICMP_PARAMETERPROB] = {//参数有误
                .handler = icmp_unreach,
                .error = 1
        },
        [ICMP_TIMESTAMP] = {//时间戳请求
                .handler = icmp_timestamp,
                .error = 1
        },
        [ICMP_TIMESTAMPREPLY] = {//时间戳应答
                .handler = icmp_discard,
        },
        [ICMP_INFO_REQUEST] = {//信息请求
                .handler = icmp_discard,
        },
        [ICMP_INFO_REPLY] = {//信息应答
                .handler = icmp_discard,
        },
        [ICMP_ADDRESS] ={//ip地址掩码请求
                .handler = icmp_address,
        },
        [ICMP_ADDRESS_REPLY]={//IP地址掩码应答
                .handler = icmp_address_reply,
        }




};


#endif //SIP_SIP_ICMP_H
