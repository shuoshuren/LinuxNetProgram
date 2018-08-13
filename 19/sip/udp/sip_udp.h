//
// Created by xc on 18-8-8.
// UDP协议
//

#ifndef SIP_SIP_UDP_H
#define SIP_SIP_UDP_H

#include "../skbuff/sip_skbuff.h"
#include "../ether/sip_ether.h"
#include "../socket/sip_socket.h"

#define UDP_FLAGS_NOCHKSUM 0x0000

struct sip_udphdr{
    __uint16_t source;//源端口
    __uint16_t dest;//目的端口
    __uint16_t len;//数据长度
    __uint16_t check;//udp校验和
};

struct udp_pcb{
    struct in_addr ip_local;//本地IP地址
    __uint16_t port_local;//本地端口地址
    struct in_addr ip_remote;//目的IP地址
    __uint16_t port_retmote;//目的端口地址
    __uint8_t tos;//服务类型
    __uint8_t ttl;//生存时间
    __uint8_t flags;//状态标记
    struct sock *sock;//网络无关结构
    struct udp_pcb *next;//下一个udp控制单元
    struct udp_pcb *prev;//前一个udp控制单元
};

#define UDP_HTABLE_SIZE 128 //UDP控制单元大小
static struct udp_pcb *udp_pcbs[UDP_HTABLE_SIZE];

/**
 * 端口分配方法
 * @return
 */
static __uint16_t found_a_port();

/**
 * udp输入函数
 * @param dev
 * @param skb
 */
int sip_udp_input(struct net_device *dev, struct skbuff *skb);

/**
 * 输出函数
 * @param dev
 * @param skb
 * @param pcb
 * @param src
 * @param dest
 * @return
 */
int sip_udp_send_output(struct net_device *dev, struct skbuff *skb,
        struct udp_pcb *pcb,struct in_addr *src,struct in_addr *dest);

/**
 * 数据报文层建立函数
 * @return
 */
struct udp_pcb * sip_udp_new(void);

/**
 * 数据报文层释放函数
 */
void sip_udp_remove(struct udp_pcb *pcb);

/**
 * udp 绑定函数
 * @param pcb
 * @param ipaddr
 * @param port
 * @return
 */
int sip_udp_bind(struct udp_pcb *pcb,struct in_addr *ipaddr,__uint16_t port);

/**
 * 发送数据函数
 * @param dev
 * @param pcb
 * @param skb
 * @param dst_ip
 * @param dst_port
 * @return
 */
int sip_udp_sendto(struct net_device *dev,struct udp_pcb *pcb,struct skbuff *skb,
        struct in_addr *dst_ip,__uint16_t dst_port);

/**
 * udp 校验报文
 * @param skb
 * @param src
 * @param dest
 * @param proto
 * @param proto_len
 * @return
 */
__uint16_t sip_chksum_pseudo(struct skbuff *skb, struct in_addr *src,
        struct in_addr *dest, __uint8_t proto, __uint16_t proto_len);


#endif //SIP_SIP_UDP_H
