//
// Created by xc on 18-8-8.
//

#include <stdio.h>
#include <arpa/inet.h>
#include <memory.h>
#include <time.h>

#include "sip_ip.h"
#include "../skbuff/sip_skbuff.h"
#include "../icmp/sip_icmp.h"
#include "../udp/sip_udp.h"


/**
 * ip层输入处理
 * @param dev
 * @param skb
 * @return
 */
int ip_input(struct net_device *dev, struct skbuff *skb){
    printf("==>ip input\n");
    struct sip_iphdr *iph = skb->nh.iph;
    int retval = 0;
    if(skb->len<0){//网络数据长度不合法
        skb_free(skb);
        retval = -1;
        goto EXITip_input;
    }
    if(iph->version != 4){//ip 版本不合适
        skb_free(skb);
        retval = -1;
        goto EXITip_input;
    }
    __uint16_t hlen = iph->ihl<<2;//计算IP头部长度
    if(hlen<IPHDR_LEN){//长度过小
        skb_free(skb);
        retval = -1;
        goto EXITip_input;
    }
    if(skb->tot_len-ETH_HLEN <ntohs(iph->tot_len)){//计算总长度是否合法
        skb_free(skb);
        retval = -1;
        goto EXITip_input;
    }
    if(hlen<ntohs(iph->tot_len)){//头部长度是否合法
        skb_free(skb);
        retval = -1;
        goto EXITip_input;
    }
    if(sip_chksum(skb->nh.raw,IPHDR_LEN)){//计算IP头部的校验和释放正确
        printf("ip check sum error\n");
        skb_free(skb);
        retval = -1;
        goto EXITip_input;
    }else{
        skb->ip_summed = CHECKSUM_HW;//设置IP校验标记
        printf("ip check sum success\n");
    }
    if((iph->daddr!=dev->ip_host.s_addr //不是发往本地
    && !ip_is_broadcast(dev,iph->daddr) //目的地址不是广播地址
    || ip_is_broadcast(dev,iph->saddr))){//源地址不是广播地址
        printf("ip address invalid\n");
        skb_free(skb);
        retval = -1;
        goto EXITip_input;
    }
    if((ntohs(iph->frag_off) &0x3FFF) != 0){//有偏移，是一个分片
        skb = sip_reassemble(skb);//进行分片重组
        if(!skb){
            retval = 0;
            goto EXITip_input;
        }
    }

    switch(iph->protocol){//IP协议类型
        case IPPROTO_ICMP://ICMP协议
            skb->th.icmph = (struct sip_icmphdr*)skb_put(skb,sizeof(struct sip_icmphdr));
            icmp_input(dev,skb);
            break;
        case IPPROTO_UDP://UDP协议
            skb->th.udph = (struct sip_udphdr*)skb_put(skb,sizeof(struct sip_udphdr));
            sip_udp_input(dev,skb);
            break;
        default:
            break;
    }



EXITip_input:
    printf("<==ip input\n");
    return retval;
}



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
              struct in_addr *dest,__uint8_t ttl,__uint8_t tos,__uint8_t proto){
    struct sip_iphdr *iph = skb->nh.iph;
    iph->protocol = proto;
    iph->tos = tos;
    iph->ttl = ttl;
    iph->daddr = dest->s_addr;
    iph->saddr = src->s_addr;
    iph->check = 0;
    iph->check = (sip_chksum(skb->nh.raw,sizeof(struct sip_iphdr)));

    if(sip_chksum(skb->nh.raw,sizeof(struct sip_iphdr))){
        printf("icmp check ip sum error\n");
    }else{
        printf("icmp check ip sum success\n");
    }
    skb->len = skb->tot_len;
    if(skb->len>dev->mtu){//如果网络数据超过以太网的mtu
        skb = ip_frag(dev,skb);
    }

    dev->output(skb,dev);
    return 0;
}

/**
 * 分片
 * @param dev
 * @param skb
 * @return
 */
struct skbuff *ip_frag(struct net_device *dev, struct skbuff *skb) {
    __uint8_t frag_num =0;
    __uint16_t tot_len = ntohs(skb->nh.iph->tot_len);
    __uint8_t mtu = dev->mtu;
    __uint8_t half_mtu = (mtu+1)/2;
    frag_num = (tot_len - IPHDR_LEN +half_mtu) /(mtu - IPHDR_LEN - ETH_HLEN);//计算分片的个数
    __uint16_t i = 0;
    struct skbuff *skb_h = NULL,*skb_t = NULL,*skb_c = NULL;
    for (i = 0,  skb->tail = skb->head ;i<frag_num;i ++) {
        if(i == 0){
            skb_t = skb_alloc(mtu);
            skb_t->phy.raw = skb_put(skb_t,ETH_HLEN);
            skb_t->nh.raw = skb_put(skb_t,IPHDR_LEN);

            memcpy(skb_t->head,skb->head,mtu);
            skb_put(skb,mtu);
            skb_t->nh.iph->frag_off = htons(0x2000);
            skb_t->nh.iph->tot_len = htons(mtu-ETH_HLEN);
            skb_t->nh.iph->check = 0;
            skb_t->nh.iph->check = sip_chksum(skb_t->nh.raw,IPHDR_LEN);
            skb_h = skb_c = skb_t;
        }else if(i == frag_num -1){
            skb_t->phy.raw = skb_put(skb_t,ETH_HLEN);
            skb_t->nh.raw = skb_put(skb_t,IPHDR_LEN);

            memcpy(skb_t->head,skb->head,ETH_HLEN+IPHDR_LEN);
            memcpy(skb_t->head+ETH_HLEN+IPHDR_LEN,skb->tail,skb->end-skb->tail);
            skb_t->nh.iph->frag_off = htons(i*(mtu - ETH_HLEN-IPHDR_LEN)+IPHDR_LEN);
            skb_t->nh.iph->tot_len = htons(skb->end-skb->tail+IPHDR_LEN);
            skb_t->nh.iph->check = 0;
            skb_t->nh.iph->check = sip_chksum(skb_t->nh.raw,IPHDR_LEN);
            skb_c->next = skb_t;
        }else{
            skb_t = skb_alloc(mtu);
            skb_t->phy.raw = skb_put(skb_t,ETH_HLEN);
            skb_t->nh.raw = skb_put(skb_t,IPHDR_LEN);

            memcpy(skb_t->head,skb->head,ETH_HLEN+IPHDR_LEN);
            memcpy(skb_t->head+ETH_HLEN+IPHDR_LEN,skb->tail,mtu-ETH_HLEN-IPHDR_LEN);
            skb_put(skb_t,mtu-ETH_HLEN-IPHDR_LEN);
            skb_t->nh.iph->frag_off = htons((i*(mtu-ETH_HLEN-IPHDR_LEN)+IPHDR_LEN)|0x2000);
            skb_t->nh.iph->tot_len = htons(mtu-ETH_HLEN);
            skb_t->nh.iph->check = 0;
            skb_t->nh.iph->check = sip_chksum(skb_t->nh.raw,IPHDR_LEN);
            skb_c->next = skb_t;
            skb_c = skb_t;
        }
        skb_t->ip_summed = 1;//已经进行了IP校验和计算
    }
    skb_free(skb);//是否原来的网络数据
    return skb_h;//返回分片的头部指针



    return NULL;
}

/**
 * ip 分片组装
 * @param skb
 * @return
 */
struct skbuff *sip_reassemble(struct skbuff *skb) {
    struct sip_iphdr *fraghdr = skb->nh.iph;
    int retval = 0;
    __uint16_t offset,len;
    int found = 0;
    offset = (fraghdr->frag_off&0x1FFF) << 3;//取得IP分钟偏移地址
    len = fraghdr->tot_len - fraghdr->ihl<<2;
    struct sip_reass *ipr = NULL,*ipr_prev = NULL;
    for (ipr_prev = ipr = ip_reass_list; ipr != NULL; ) {
        if(time(NULL) - ipr->timer >IPREASS_TIMEOUT){
            if(ipr_prev == NULL){//第一个分片
                ipr_prev = ipr;
                ip_reass_list->next = ipr = ipr->next;//将超时的分片从重组链表中取下来
                ipr = ipr->next;
                IP_FREE_REASS(ipr_prev);
                ipr_prev ->next = NULL;
                continue;

            }else{
                ipr_prev ->next = ipr->next;//从分片链表中摘除当前链
                IP_FREE_REASS(ipr);
                ipr = ipr_prev->next;
                continue;
            }
        }
        if(ipr->iphdr.daddr == fraghdr->daddr/*目的IP地址匹配*/
            && ipr->iphdr.saddr == fraghdr->saddr /*源IP地址匹配*/
            && ipr->iphdr.id == fraghdr->id /*分片id匹配*/){
            found = 1;
            break;
        }

    }
    if(!found){//没有找到
        ipr_prev = NULL;
        ipr = (struct sip_reass*)malloc(sizeof(struct sip_reass));
        if(!ipr){
            retval = -1;
            goto freeskb;
        }
        memset(ipr,0,sizeof(struct sip_reass));
        ipr->next = ip_reass_list;//将当前分钟结构挂接到分组链的头部
        ip_reass_list = ipr;
        memcpy(&ipr->iphdr,skb->nh.raw,sizeof(IPHDR_LEN));//复制IP的数据头部，便于之后分片匹配

    }else{//找到合适的分组链
        if(((fraghdr->frag_off & 0x1FFF) ==0) //当前数据位于分片第一个
            &&((ipr->iphdr.frag_off & 0x1FFF) != 0)) {//分组链上头部不是第一个分片
            memcpy(&ipr->iphdr, fraghdr, IPHDR_LEN);//更新重组的IP头部结构
        }
    }
    if((fraghdr->frag_off & htons(0x2000)) == 0){//检查最后一个分组，没有更多的分组
#define IP_REASS_FLAG_LASTFRAG 0x01
        ipr->flags != IP_REASS_FLAG_LASTFRAG;//设置最后分组标志
        ipr->datagram_len = offset+len;

    }
    struct skbuff *skb_prev = NULL,*skb_cur = NULL;
    int finish = 0;
    void *pos = NULL;
    __uint32_t length = 0;
#define FRAG_OFFSET(iph) (ntohs(iph->frag_off &0x1FFF)<<3)
#define FRAG_LENGTH(iph) (ntohs(iph->tot_len) - IPHDR_LEN)
    for(skb_prev = NULL,skb_cur = ipr->skb,length = 0,found = 0;
        skb_cur != NULL && !found;
        skb_prev = skb_cur,skb_cur = skb_cur->next){
        if(skb_prev != NULL){//不是第一个分片

            if((offset < FRAG_OFFSET(skb_cur->nh.iph))//接收数据偏移值位于前后两个之间
                && (offset>FRAG_OFFSET(skb_prev->nh.iph))){
                skb->next = skb_cur;
                skb_prev->next = skb;
                if(offset+len >FRAG_OFFSET(skb_cur->nh.iph)){//当前数据与后面分片数据覆盖
                    __uint16_t modify = FRAG_OFFSET(skb_cur->nh.iph) - offset+IPHDR_LEN;
                    skb->nh.iph->tot_len = htons(modify);//更新当前链长度
                }
                if(FRAG_OFFSET(skb_prev->nh.iph) + FRAG_LENGTH(skb_prev->nh.iph)
                > FRAG_OFFSET(skb_cur->nh.iph)){//前面的分片长度覆盖当前数据
                    __uint16_t modify = FRAG_OFFSET(skb_prev->nh.iph) - offset+IPHDR_LEN;
                    skb_prev->nh.iph->tot_len = htons(modify);
                }
                found = 1;
            }
        }else{//是重组链的头部
            if(offset<FRAG_OFFSET(skb_cur->nh.iph)){
                skb->next = ipr->skb;
                ipr->skb = skb;
                if(offset+len+IPHDR_LEN > FRAG_OFFSET(skb_cur->nh.iph)){
                    __uint16_t modify = FRAG_OFFSET(skb_cur->nh.iph) - offset+IPHDR_LEN;
                    if(!offset){
                        modify -= IPHDR_LEN;
                    }
                    skb->nh.iph->tot_len = htons(modify);
                }
            }
        }
        length += skb_cur->nh.iph->tot_len - IPHDR_LEN;//当前链表中数据长度
    }
    for(skb_cur = ipr->skb,length = 0;
        skb_cur != NULL;
        skb_cur = skb_cur->next){
        length += skb_cur->nh.iph->tot_len - IPHDR_LEN;
    }
    length += IPHDR_LEN;
    //全部ip分片都已经进行重新组合，复制到新的数据结构中
    if(length == ipr->datagram_len){
        ipr->datagram_len += IPHDR_LEN;
        skb = skb_alloc(ipr->datagram_len+ETH_HLEN);
        skb->phy.raw = skb_put(skb,ETH_HLEN);
        skb->nh.raw = skb_put(skb,IPHDR_LEN);
        memcpy(skb->nh.raw,& ipr->iphdr,sizeof(ipr->iphdr));
        skb->nh.iph->tot_len = htons(ipr->datagram_len);
        for(skb_prev = skb_cur = ipr->skb;skb_cur != NULL;){
            int size = skb_cur->end - skb_cur->tail;
            pos = skb_put(skb,size);
            memcpy(pos,skb_cur->tail,skb_cur->nh.iph->tot_len - skb_cur->nh.iph->ihl<<2);
        }
        ipr_prev->next = ipr->next;
        IP_FREE_REASS(ipr);
        skb->nh.iph->check = 0;
        skb->nh.iph->frag_off = 0;
        skb->nh.iph->check = sip_chksum(skb->nh.raw,skb->nh.iph->tot_len);
    }
normal:
    return skb;
freeskb:
    skb_free(skb);

    return NULL;
}

/**
 * 判断IP是否是广播IP
 * @param dev
 * @param ip
 * @return
 */
bool ip_is_broadcast(struct net_device *dev, __uint32_t ip) {
    int retval = 1;
    if(ip == IP_ADDR_ANY_VALUE || (~ip == IP_ADDR_ANY_VALUE)){//本地任意IP
        printf("ip is any ip\n");
        retval = 1;
        goto EXITin_addr_isbroadcast;
    }else if(ip == dev->ip_host.s_addr){//本地地址
        printf("ip is local ip\n");
        retval = 0;
        goto EXITin_addr_isbroadcast;
    }else if(((ip&dev->ip_netmask.s_addr) == (dev->ip_host.s_addr&dev->ip_netmask.s_addr))
    &&((ip &~dev->ip_netmask.s_addr) == (IP_ADDR_BROADCAST_VALUE &~dev->ip_netmask.s_addr))){
        printf("ip is any ip");
        retval = 1;
        goto EXITin_addr_isbroadcast;
    }else{
        retval = 0;
    }

EXITin_addr_isbroadcast:

    return retval;
}

/**
 * IP是否是多播
 * @param dev
 * @param ip
 * @return
 */
bool ip_is_multicast( __uint32_t ip){
    return false;
}

bool sip_chksum(__uint8_t *raw, int len) {
    return 0;
}