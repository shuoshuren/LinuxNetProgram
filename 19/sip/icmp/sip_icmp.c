//
// Created by xc on 18-8-8.
//
#include <stdio.h>
#include <stdlib.h>
#include "sip_icmp.h"
#include "../ip/sip_ip.h"
#include "../skbuff/sip_skbuff.h"

/**
 * 输入函数
 * @param dev
 * @param skb
 */
static void icmp_input(struct net_device *dev, struct skbuff *skb){
    printf("==>icmp input\n");
    struct sip_icmphdr *icmph;
    switch(skb->ip_summed){//查看释放进行了校验和计算
        case 0:{
            skb->csum = 0;
            if(sip_chksum(skb->phy.raw,0)){
                printf("icmp checksum error\n");
                goto drop;
            }
            break;
        }
        default:
            break;
    }
    icmph = skb->th.icmph;
    if(icmph->type>NR_ICMP_TYPES){//类型不对
        goto drop;
    }
    icmp_pointers[icmph->type].handler(dev,skb);//查找icmp_pointers中合适的处理函数

normal:
    printf("<==icmp input\n");

drop:
    skb_free(skb);
    goto normal;
}


/**
 * 回显应答
 * @param dev
 * @param skb
 */
static void icmp_echo(struct net_device *dev,struct skbuff *skb){
    printf("==>icmp echo \n");
    struct sip_icmphdr *icmph = skb->th.icmph;
    struct sip_iphdr *iph = skb->nh.iph;
    printf("tot_len:%d\n",skb->tot_len);
    if(ip_is_broadcast(dev,skb->nh.iph->daddr) || ip_is_multicast(skb->nh.iph->daddr) ){
        goto EXITicmp_echo;
    }
    icmph->type = ICMP_ECHOREPLY;
    if(icmph->checksum >= htons(0xFFFF-(ICMP_ECHO<<8))){
        icmph->checksum += htons(ICMP_ECHO<<8) +1;

    }else{
        icmph->checksum += htons(ICMP_ECHO<<8);
    }

    __uint32_t dest = skb->nh.iph->saddr;
    ip_output(dev,skb,&dev->ip_host.s_addr,&dest,255,0,IPPROTO_ICMP);
EXITicmp_echo:
    printf("<==icmp echo \n");
    return;
}