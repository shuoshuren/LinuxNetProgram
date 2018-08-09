//
// Created by xc on 18-8-8.
// ARP协议
//

#ifndef SIP_SIP_ARP_H
#define SIP_SIP_ARP_H

#include <zconf.h>
#include "../skbuff/sip_skbuff.h"
#include "../ether/sip_ether.h"

#define ARP_ESTABLISHED 0

struct sip_arphdr{
    __uint32_t ar_tip;
};

struct arpt_arp{
    __uint8_t ethaddr[ETH_ALEN];
};

void arp_add_entry(__uint8_t saddr, __uint8_t source[6], int i);

void arp_input(struct skbuff *skb, struct net_device *dev);

static struct arpt_arp *arp_find_entry(__uint32_t ip);

static void arp_request(struct net_device *dev, __uint32_t ip);

#endif //SIP_SIP_ARP_H
