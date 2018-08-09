//
// Created by xc on 18-8-8.
//

#include "sip_skbuff.h"
#include "../ether/sip_ether.h"
#include "../ip/sip_ip.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

/**
 * 申请用户指定大小缓存区域
 * @param size
 * @return
 */
struct skbuff * skb_alloc(__uint32_t size){
    printf("==>skb alloc\n");
    struct skbuff *skb = (struct skbuff *)malloc(sizeof(struct skbuff));
    if(!skb){
        printf("==> malloc skb header error\n");
        return NULL;
    }
    memset(skb,0,sizeof(struct skbuff));//初始化skbuff内存结构
    skb->head = (__uint8_t *) malloc(size);//申请数据区域内存，并保存在head指针中
    if(!skb->head){
        printf("==> malloc skb data error\n");
        free(skb);//释放之前生气的skbuff内存
        return NULL;
    }
    memset(skb->head,0,size);//初始化用户内存区
    skb->end = skb->head+size;//end指针初始化
    skb->data = skb->head;//data指针初始化和head一致
    skb->tail = skb->data;//tail指针最初和data一致
    skb->next = NULL;
    skb->tot_len = 0;//有用数据总长度为0
    skb->len = 0;//当前结构中数据长度为0
    printf("<==skb alloc\n");
    return skb;
}

/**
 * 释放函数
 * @param skb
 */
void skb_free(struct skbuff *skb){
    if(skb){
        if(skb->head){
            free(skb->head);
        }
        free(skb);
    }
}

/**
 * 复制skbuff
 * @param from
 * @param to
 */
void skb_clone(struct skbuff *from,struct skbuff *to){
    memcpy(to->head,from->head,from->end-from->head);//复制用户数据
    to->phy.ethh = (struct sip_ethhdr*)skb_put(to,ETH_LEN);//更改目的结构的以太网指针位置
    to->nh.iph = (struct sip_iphdr*)skb_put(to,IPHDR_LEN);//更改IP头部的指针位置
}

/**
 * 移动skbuff的tail指针
 * @param skb
 * @param len
 * @return
 */
__uint8_t skb_put(struct skbuff *skb,__uint32_t len){
    printf("==>skb put\n");
    __uint8_t *tmp = skb->tail;//保存尾部指针位置
    skb->tail += len;//移动尾部指针
    skb->len -= len;//当前网络数据长度减少

    printf("<==skb put\n");
    return tmp;
}