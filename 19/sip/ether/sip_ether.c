//
// Created by xc on 18-8-8.
//
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "sip_ether.h"
#include "../skbuff/sip_skbuff.h"
#include "../ip/sip_ip.h"
#include "../arp/sip_arp.h"


static void sip_init_ethnet(struct net_device *dev){
    printf("==>sip init ethnet\n");
    memset(dev,0,sizeof(struct net_device));

    dev->s = socket(AF_INET,SOCK_PACKET,htons(ETH_P_ALL));

    if(dev->s>0){
        printf("create sock_packet fd success\n");
    }else{
        printf("create sock_packet fd failure\n");
        exit(-1);
    }
    //将套接字绑定到网卡上
    strcpy(dev->name,"eth1");
    memset(&dev->to,'\0',sizeof(struct sockaddr));
    dev->to.sa_family = AF_INET;
    strcpy(dev->to.sa_data,dev->name);
    int r = bind(dev->s,&dev->to,sizeof(struct sockaddr));
    memset(dev->hwbroadcast,0xFF,ETH_ALEN);
    //设置mac地址
    dev->hwaddr[0] = 0x00;
    dev->hwaddr[1] = 0x0c;
    dev->hwaddr[2] = 0x29;
    dev->hwaddr[3] = 0x40;
    dev->hwaddr[4] = 0x0a;
    dev->hwaddr[5] = 0x07;
    dev->hwaddr_len = ETH_ALEN;
    dev->ip_host.s_addr = inet_addr("192.168.59.129");
    dev->ip_gw.s_addr = inet_addr("192.168.59.1");
    dev->ip_netmask.s_addr = inet_addr("255.255.255.0");
    dev->ip_broadcast.s_addr = inet_addr("192.168.59.255");

    dev->input = input;
    dev->output = output;
    dev->linkoutput = lowoutput;
    dev->type = ETH_P_802_3;

    printf("<==sip init ethnet\n");
}


/**
     * 用于从网络设备中获取数据传入网络协议栈进行处理
     * @param skb
     * @param dev
     * @return
     */
static __uint8_t * input(struct skbuff *pskb,struct net_device *dev){
    printf("==>input \n");
    char ef[ETH_FRAM_LEN];//以太帧缓冲
    int n,i;
    int retval = 0;
    //读取以太网数据，n为返回的实际捕获的以太帧的长度
    n = read(dev->s,ef,ETH_FRAM_LEN);
    if(n<0){
        printf("Not datum\n");
        retval = -1;
        return retval;
    }else{
        printf("%d bytes datum\n",n);
    }
    //申请存放刚才读取的数据的空间
    struct skbuff *skb = skb_alloc(n);
    if(!skb){
        retval = -1;
        return retval;
    }

    memcpy(skb->head,ef,n);//将接受的网络数据复制到skb上
    skb->tot_len=skb->len = n;
    skb->phy.ethh = (struct sip_ethhdr*)skb_put(skb,sizeof(struct sip_ethhdr));//获取以太网头部指针
    if(samemac(skb->phy.ethh->h_dest,dev->hwaddr/*数据发送到本机*/
    || samemac(skb->phy.ethh->h_dest,dev->hwbroadcast/*广播数据*/))){
        switch(htons(skb->phy.ethh->h_proto)){
            case ETH_P_IP://IP类型
                skb->nh.iph = (struct sip_iphdr*)skb_put(skb,sizeof(struct sip_iphdr));//获取ip的头指针
                //将刚才接收到的网络数据用来更新ARP表中映射关系
                arp_add_entry(skb->nh.iph->saddr,skb->phy.ethh->h_source,ARP_ESTABLISHED);
                ip_input(dev,skb);//交给IP层处理
                break;
            case ETH_P_ARP://ARP类型
                skb->nh.arph = (struct sip_arphdr*)skb_put(skb,sizeof(struct sip_arphdr));
                if(*((__uint32_t*)skb->nh.arph->ar_tip) == dev->ip_host.s_addr){
                    arp_input(skb,dev);//ARP模块处理接收
                }
                skb_free(skb);
                break;
            default:
                printf("ETHER: UNKNOWN\n");
                skb_free(skb);
                break;
        }
    }else{
        skb_free(skb);
    }
    printf("<==input \n");
    return 0;
}


/**
 * 用于IP模块发送数据时候调用，会先调用ARP模块对IP地址进行查找，然后发送数据
 * @param skb
 * @param dev
 * @return
 */
static __uint8_t * output(struct skbuff *skb,struct net_device *dev){
    printf("==>output\n");
    int retval = 0;
    struct arpt_arp *arp = NULL;
    int times =0,found = 0;
    //发送网络数据的目的IP为skbuff所指的目的IP
    __uint32_t destip = skb->nh.iph->daddr;
    //判断目的IP和本机是否在同一个子网上
    if((skb->nh.iph->daddr & dev->ip_netmask.s_addr)
    != (dev->ip_host.s_addr&dev->ip_netmask.s_addr)){
        destip = dev->ip_gw.s_addr;//不在同一个子网上，将数据发送给网关
    }
    //分5次查找目的主机的mac地址
    while ((arp = arp_find_entry(destip)) == NULL && times<5){
        //发送arp请求
        arp_request(dev,destip);
        sleep(1);
        times++;
    }
    if(!arp){//没有找到对应的mac地址
        retval = 1;
        return retval;
    }else{//找到了
        struct sip_ethhdr *eh = skb->phy.ethh;
        memcpy(eh->h_dest,arp->ethaddr,ETH_ALEN);//设置目的mac地址为对应项中的值
        memcpy(eh->h_source,dev->hwaddr,ETH_ALEN);//设置源mac为本机mac地址
        eh->h_proto = htons(ETH_P_IP);//以太网的协议类型设置为IP
        dev->linkoutput(skb,dev);//发送数据
    }

    printf("<==output\n");
    return retval;
}


/**
 * 由ARP模块调用，直接发送网络数据
 * @param skb
 * @param dev
 * @return
 */
static __uint8_t * lowoutput(struct skbuff *skb,struct net_device *dev){
    printf("==> lowoutput\n");
    int n=0;
    int len = sizeof(struct sockaddr);
    struct skbuff *p = NULL;
    //将skbuff链结构中网络数据发送出去
    for(p = skb;p!=NULL;skb=p,p=p->next,skb_free(skb),skb = NULL){
        n = sendto(dev->s,skb->head,skb->len,0,&dev->to,len);
        printf("Send Number,n:%d\n",n);
    }
    printf("<== lowoutput\n");
    return 0;
}

/**
 * 判断是否是相同的mac
 */
int samemac(__uint8_t *mac1,__uint8_t *mac2){

    for(int i=0;i<ETH_ALEN;i++){
        if(mac1[i] != mac2[i]){
            return 0;
        }
    }
    return 1;
}