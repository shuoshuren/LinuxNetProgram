//
// Created by xc on 18-8-8.
//

#include <memory.h>
#include <time.h>
#include <stdio.h>
#include "sip_arp.h"
#include "../skbuff/sip_skbuff.h"

/**
 * 初始化ARP映射表
 */
void init_arp_entry(){
    int i=0;
    for(i = 0;i<ARP_TABLE_SIZE;i++){
        arp_table[i].ctime = 0;
        memset(arp_table[i].ethaddr,0,ETH_ALEN);
        arp_table[i].ipaddr = 0;
        arp_table[i].status = ARP_EMPTY;
    }
}

/**
 * 查找摸IP对应的映射表项
 * @param ip
 * @return
 */
static struct arpt_arp *arp_find_entry(__uint32_t ip){
    int i = -1;
    struct arpt_arp *found = NULL;
    for (i = 0; i < ARP_TABLE_SIZE; i++) {
        if(arp_table[i].ctime > time(NULL) +ARP_LIVE_TIME){//查看是否超时
            arp_table[i].status = ARP_EMPTY;//超时，置空表项
        }else if(arp_table[i].ipaddr == ip && arp_table[i].status == ARP_ESTABLISHED){
            found = &arp_table[i];
            break;
        }
    }
    return found;
}

/**
 * 更新映射表中某个IP地址的mac地址映射
 * @param ip
 * @param ethaddr
 * @return
 */
struct arpt_arp * update_arp_entry(__uint32_t ip,__uint8_t *ethaddr){
    struct arpt_arp *found = NULL;
    found = arp_find_entry(ip);//根据IP查找ARP表项
    if(found){
        memcpy(found->ethaddr,ethaddr,ETH_ALEN);
        found->status = ARP_ESTABLISHED;
        found->ctime = time(NULL);
    }
    return found;
}

/**
 * 向映射表项中增加新的映射对，如果IP对应表项已存在，则对mac地址和时间戳进行更新
 * @param ip
 * @param ethaddr
 * @param status
 */
void arp_add_entry(__uint32_t ip, __uint8_t* ethaddr, enum arp_status status){
    int i = 0;
    struct arpt_arp *found = NULL;
    found = update_arp_entry(ip,ethaddr);//更新ARP表项
    if(!found){
        for ( i = 0; i < ARP_TABLE_SIZE; ++i) {
            if(arp_table[i].status == ARP_EMPTY){
                found = &arp_table[i];//重置found变量
                break;
            }
        }
    }
    if(found){
        found->ipaddr = ip;
        memcpy(found->ethaddr,ethaddr,ETH_LEN);
        found->status = status;
        found->ctime = time(NULL);
    }
}

/**
 * 创建一个ARP请求
 * @param dev
 * @param type
 * @param src_ip
 * @param dest_ip
 * @param src_hw
 * @param dest_hw
 * @param target_hw
 * @return
 */
struct skbuff *arp_create(struct net_device *dev,int type,__uint32_t src_ip,
                          __uint32_t dest_ip,__uint8_t *src_hw,__uint8_t *dest_hw,__uint8_t *target_hw){
    struct skbuff *skb;
    struct sip_arphdr *arph;
    printf("==> arp create\n");
    //请求skbuff结构内存，大小为一个最小的以太网帧，60字节
    skb = skb_alloc(ETH_ZLEN);
    if(skb == NULL){
        return NULL;
    }

    skb->phy.raw = skb_put(skb,sizeof(struct sip_ethhdr));//更新物理层头部指针位置
    skb->nh.raw = skb_put(skb,sizeof(struct sip_arphdr));//更新网络层头部指针位置
    arph = skb->nh.arph;//设置arp头部指针
    skb->dev = dev;//设置网络设备指针
    if(src_hw == NULL){
        src_hw = dev->hwaddr;//源地址设置为网络设备的硬件地址
    }
    if(dest_hw == NULL){
        dest_hw = dev->hwbroadcast;//目的地址设置为以太网广播硬件地址
    }
    skb->phy.ethh->h_proto = htons(ETH_P_ARP);//物理层网络协议设置为ARP协议
    memcpy(skb->phy.ethh->h_dest,dest_hw,ETH_ALEN);
    memcpy(skb->phy.ethh->h_source,src_hw,ETH_ALEN);

    arph->ar_op = htons(type);
    arph->ar_hrd = htons(ETH_P_802_3);
    arph->ar_pro = htons(ETH_P_IP);
    arph->ar_hln = ETH_ALEN;
    memcpy(arph->ar_sha,src_hw,ETH_ALEN);
    memcpy(arph->ar_sip,(__uint8_t*)&src_ip,4);
    memcpy(arph->ar_tip,(__uint8_t*)&dest_ip,4);
    if(target_hw != NULL){
        memcpy(arph->ar_tha,target_hw,dev->hwaddr_len);
    }else{
        memset(arph->ar_tha,0,dev->hwaddr_len);
    }

    printf("<== arp create\n");
    return skb;
}

/**
 * 从网络接口层输入的网络数据进行解析，更新ARP映射表，并响应其他主机的ARP请求
 * @param pskb
 * @param dev
 */
int arp_input(struct skbuff **pskb, struct net_device *dev){
    struct skbuff *skb = *pskb;
    __uint32_t ip = 0;
    printf("==>arp_input \n");
    if(skb->tot_len<sizeof(struct sip_arphdr)){//接受的网络数据总长度小于ARP头部长度
        goto EXITarp_input;
    }

    ip = *(__uint32_t *)(skb->nh.arph->ar_tip);//arp 请求的目的地址
    if(ip == dev->ip_host.s_addr){//如果是本机地址
        update_arp_entry(ip,dev->hwaddr);//更新ARP表
    }
    struct in_addr t_addr;
    switch(ntohs(skb->nh.arph->ar_op)){
        case ARPOP_REQUEST://ARP请求类型

            t_addr.s_addr = *(unsigned int *)skb->nh.arph->ar_sip;//ARP请求源IP地址
            printf("ARPOP_REQUEST,from:%s\n",inet_ntoa(t_addr));
            //向ARP请求的IP地址发送应答
            arp_send(dev,ARPOP_REPLY,dev->ip_host.s_addr,*(__uint32_t *)skb->nh.arph->ar_sip,
                    dev->hwaddr,skb->phy.ethh->h_source,skb->nh.arph->ar_sha);
            //将此项ARP映射加入到映射表
            arp_add_entry(*(__uint32_t*)skb->nh.arph->ar_sip,skb->phy.ethh->h_source,ARP_ESTABLISHED);
            break;
        case ARPOP_REPLY://arp应答类型
            arp_add_entry(*(__uint32_t*)skb->nh.arph->ar_sip,skb->phy.ethh->h_source,ARP_ESTABLISHED);
            break;
    }

EXITarp_input:
    printf("<==arp_input \n");
    return 0;

}

/**
 *
 * @param dev 设备
 * @param type ARP协议类型
 * @param src_ip 源主机IP
 * @param dest_ip 目的主机IP
 * @param src_hw  源主机mac
 * @param dest_hw  目的主机mac
 * @param target_hw  解析主机mac
 */
void arp_send(struct net_device *dev, int type, __uint32_t src_ip,
              __uint32_t dest_ip, __uint8_t *src_hw, __uint8_t *dest_hw,
              __uint8_t *target_hw){
    struct skbuff *skb;
    printf("==>arp send\n");
    //建立一个ARP网络报文
    skb = arp_create(dev,type,src_ip,dest_ip,src_hw,dest_hw,target_hw);
    if(skb){
        dev->linkoutput(skb,dev);//调用底层的网络发送函数
    }
    printf("<==arp send\n");
}


/**
 * 查询ARP映射表为空的IP地址对应的mac地址，项局域网内广播请求，然后由arp_input()函数记录主机响应信息
 * @param dev
 * @param ip
 */
int arp_request(struct net_device *dev, __uint32_t ip){
    struct skbuff *skb;
    printf("==>arp request\n");
    __uint32_t tip = 0;
    //查看请求的IP地址和本机的IP地址释放在同一个子网上
    if((ip&dev->ip_netmask.s_addr) == (dev->ip_host.s_addr & dev->ip_netmask.s_addr)){
        tip = ip;
    }else{
        tip = dev->ip_gw.s_addr;//目的IP为网关地址
    }
    //建立一个arp报文
    skb = arp_create(dev,ARPOP_REQUEST,dev->ip_host.s_addr,
            tip,dev->hwaddr,NULL,NULL);
    if(skb){
        dev->linkoutput(skb,dev);
    }
    printf("<==arp request\n");
}