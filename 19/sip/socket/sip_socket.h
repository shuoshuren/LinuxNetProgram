//
// Created by xc on 18-8-8.
// 用户接口
//

#ifndef SIP_SIP_SOCKET_H
#define SIP_SIP_SOCKET_H

#include <bits/semaphore.h>
#include "../skbuff/sip_skbuff.h"


struct sock{
    struct skbuff *skb_recv;
     sem_t *sem_recv;
};

#endif //SIP_SIP_SOCKET_H
