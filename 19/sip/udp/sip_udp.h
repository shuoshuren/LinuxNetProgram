//
// Created by xc on 18-8-8.
// UDP协议
//

#ifndef SIP_SIP_UDP_H
#define SIP_SIP_UDP_H

#include "../skbuff/sip_skbuff.h"
#include "../ether/sip_ether.h"

struct sip_udphdr{

};

void sip_udp_input(struct net_device *dev, struct skbuff *skb);
#endif //SIP_SIP_UDP_H
