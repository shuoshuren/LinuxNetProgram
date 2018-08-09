//
// Created by xc on 18-8-8.
// IP协议
//

#ifndef SIP_SIP_IP_H
#define SIP_SIP_IP_H

#include <zconf.h>
#include "../ether/sip_ether.h"
#include "../skbuff/sip_skbuff.h"

#define IPHDR_LEN 0

struct sip_iphdr{
    __uint8_t saddr;
    __uint8_t daddr;

};

void ip_input(struct net_device *dev, struct skbuff *skb);

#endif //SIP_SIP_IP_H
