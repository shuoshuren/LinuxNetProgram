//
// Created by xc on 18-8-8.
// ARP协议
//

#ifndef SIP_SIP_ARP_H
#define SIP_SIP_ARP_H

#include <zconf.h>
#include "../skbuff/sip_skbuff.h"
#include "../ether/sip_ether.h"

#define ARP_TABLE_SIZE 10
#define ARP_LIVE_TIME 20

#define ARPOP_REQUEST 0x01
#define ARPOP_REPLY 0x02

enum arp_status{
    ARP_EMPTY,//状态为空
    ARP_ESTABLISHED//映射表项建立
};

/**
 * ARP
 */
struct sip_arphdr{
    __uint16_t ar_hrd;//硬件地址类型
    __uint16_t ar_pro;//协议类型
    __uint8_t ar_hln;//硬件地址长度
    __uint8_t ar_pln;//协议地址长度
    __uint16_t ar_op;//操作方式
    __uint8_t ar_sha[ETH_ALEN];//发送方硬件地址
    __uint8_t ar_sip[4];//发送方IP
    __uint8_t ar_tha[ETH_ALEN];//接收方硬件地址
    __uint8_t ar_tip[4];//接收方IP
};

/**
 * ARP 映射表结构是对ARP层IP地址和mac地址对应关系的映射
 */
struct arpt_arp{
    __uint32_t ipaddr;//IP
    __uint8_t ethaddr[ETH_ALEN];//MAC
    time_t ctime;//最后更新时间
    enum arp_status status;//ARP状态值
};

struct arpt_arp arp_table[ARP_TABLE_SIZE];

/**
 * 初始化ARP映射表
 */
void init_arp_entry();

/**
 * 向映射表项中增加新的映射对，如果IP对应表项已存在，则对mac地址和时间戳进行更新
 * @param ip
 * @param ethaddr
 * @param status
 */
void arp_add_entry(__uint32_t ip, __uint8_t *ethaddr, enum arp_status status);


/**
 * 更新映射表中某个IP地址的mac地址映射
 * @param ip
 * @param ethaddr
 * @return
 */
struct arpt_arp * update_arp_entry(__uint32_t ip,__uint8_t *ethaddr);

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
        __uint32_t dest_ip,__uint8_t *src_hw,__uint8_t *dest_hw,__uint8_t *target_hw);
/**
 * 从网络接口层输入的网络数据进行解析，更新ARP映射表，并响应其他主机的ARP请求
 * @param pskb
 * @param dev
 */
int arp_input(struct skbuff **pskb, struct net_device *dev);


/**
 * 查找摸IP对应的映射表项
 * @param ip
 * @return
 */
static struct arpt_arp *arp_find_entry(__uint32_t ip);

/**
 * 查询ARP映射表为空的IP地址对应的mac地址，项局域网内广播请求，然后由arp_input()函数记录主机响应信息
 * @param dev
 * @param ip
 */
int arp_request(struct net_device *dev, __uint32_t ip);

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
         __uint8_t *target_hw);

#endif //SIP_SIP_ARP_H
