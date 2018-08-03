#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h>
#include <net/tcp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include "nf_sockopte.h"

MODULE_LICENSE("Dual BSD/GPL");
/* nf 初始化状态宏 */
#define NF_SUCCESS 0
#define NF_FAILURE 1

/*初始化绑定状态*/
band_status b_status;
/*快速绑定操作宏*/
/*判断释放禁止TCP端口*/
#define IS_BANDPORT_TCP(status) \
	(status.band_port.port != 0 && status.band_port.protocol == IPPROTO_TCP)

/*判断释放禁止UDP端口*/
#define IS_BANDPORT_UDP(status) \
	(status.band_port.port != 0 && status.band_port.protocol == IPPROTO_UDP)


/*判断释放禁止ping*/
#define IS_BANDPING(status) \
	(status.band_ping)

/*判断释放禁止ip 协议*/
#define IS_BANDIP(status) \
	(status.band_ip)

/*nf sock 选项扩展操作*/
static  int  nf_sockopt_set(struct sock *sock, int cmd, void  *user, int len) {
	int ret = 0;
	struct band_status status;

	/*权限检查*/
	if (!capable(CAP_NET_ADMIN)) {
		ret = -EPERM;
		goto ERROR;
	}

	/*从用户空间复制数据*/
	ret = copy_from_user(&status, user, len);
	if (ret != 0) {
		ret = -EINVAL;
		goto ERROR;
	}

	/*命令类型*/
	switch (cmd) {
	/*设置禁止ip协议*/
	case SOE_BANDIP:
		if (IS_BANDIP(status)) {
			b_status.band_ip = status.band_ip;
		} else {
			b_status.band_ip = 0;
		}
		break;
	//设置端口禁止和相关的协议类型
	case SOE_BANDPORT:
		if (IS_BANDPORT_TCP(status)) {
			b_status.band_port.protocol = IPPROTO_TCP;
			b_status.band_port.port = status.band_port.port;
		} else if (IS_BANDPORT_UDP(status)) {
			b_status.band_port.protocol = IPPROTO_UDP;
			b_status.band_port.port = status.band_port.port;
		} else {
			b_status.band_port.protocol = 0;
			b_status.band_port.port = 0;
		}
		break;
	//禁止ping
	case SOE_BANDPING:
		if (IS_BANDPING(status)) {
			b_status.band_ping = 1;
		} else {
			b_status.band_ping = 0;
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}

ERROR:
	return ret;
}

static unsigned int nf_sockopt_get(struct sock *sock, int cmd, void __user *user, int *len) {
	int ret = 0;
	//权限检查
	if (!capable(CAP_NET_ADMIN)) {
		ret = -EPERM;
		goto ERROR;
	}

	/*将数据从内核空间复制到用户空间*/
	switch (cmd) {
	case SOE_BANDIP:
	case SOE_BANDPORT:
	case SOE_BANDPING:
		ret = copy_to_user(user, &b_status, *len);
		if (ret != 0) {
			ret = -EINVAL;
			goto ERROR;
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}

ERROR:
	return ret;
}


/*在local_out 挂载钩子*/
static unsigned int nf_hook_out(void *priv,
                                struct sk_buff *skb,
                                const struct nf_hook_state *state
                               ) {
	struct sk_buff *sb = skb;
	struct iphdr *iph = ip_hdr(sb);

	if (IS_BANDIP(b_status)) {
		if (b_status.band_ip == iph->saddr) {
			return NF_DROP;//丢弃该网络报文
		}
	}
	return NF_ACCEPT;
}

/*在local_in 挂载钩子*/
static unsigned int nf_hook_in(void *priv,
                               struct sk_buff *skb,
                               const struct nf_hook_state *state
                              ) {
	struct sk_buff *sb = skb;
	struct iphdr *iph = ip_hdr(sb);
	unsigned int src_ip = iph->saddr;
	struct tcphdr *tcph = NULL;
	struct udphdr *udph = NULL;

	switch (iph->protocol) {
	case IPPROTO_TCP://丢弃tcp
		if (IS_BANDPORT_TCP(b_status)) {
			tcph = tcp_hdr(sb);
			if (tcph->dest == b_status.band_port.port) {
				return NF_DROP;
			}
		}
		break;
	case IPPROTO_UDP://丢弃udp
		if (IS_BANDPORT_UDP(b_status)) {
			udph = udp_hdr(sb);
			if (udph->dest == b_status.band_port.port) {
				return NF_DROP;
			}
		}
		break;
	case IPPROTO_ICMP://丢弃icmp
		if (IS_BANDPING(b_status)) {
			printk(KERN_ALERT "Drop icmp packet from %d.%d.%d.%d\n",
			       (src_ip & 0xFF000000) >> 24,
			       (src_ip & 0x00FF0000) >> 16,
			       (src_ip & 0x0000FF00) >> 8,
			       (src_ip & 0x000000FF) >> 0);
			return NF_DROP;
		}
		break;
	default:
		break;
	}



	return NF_ACCEPT;
}

/*初始化nfin钩子，在钩子local_in上*/
static struct nf_hook_ops nfin = {
	.hook = nf_hook_in,
	.hooknum = NF_INET_LOCAL_IN,
	.pf = PF_INET,
	.priority = NF_IP_PRI_FIRST,
};

/*初始化nfout钩子，在钩子LOCAL_OUT上*/
static struct nf_hook_ops nfout = {
	.hook = nf_hook_out,
	.hooknum = NF_INET_LOCAL_OUT,
	.pf = PF_INET,
	.priority = NF_IP_PRI_FIRST,
};

/*初始化nf套接字选项*/
static struct nf_sockopt_ops nfsockopt = {
	.pf = PF_INET,
	.set_optmin = SOE_BANDIP,
	.set_optmax = SOE_BANDIP + 2,
	.set = nf_sockopt_set,
	.get_optmin = SOE_BANDIP,
	.get_optmax = SOE_BANDIP + 2,
	.get = nf_sockopt_get,
};

/*初始化模块*/
static int __init  nf_sockopt_init(void) {
	nf_register_net_hook(&init_net,&nfin);
	nf_register_net_hook(&init_net,&nfout);
	nf_register_sockopt(&nfsockopt);

	printk(KERN_ALERT "netfilter example 2 init successfully\n");

	return NF_SUCCESS;
}

/*清理模块*/
static void __exit nf_sockopt_exit(void) {
	nf_unregister_net_hook(&init_net,&nfin);
	nf_unregister_net_hook(&init_net,&nfout);
	nf_unregister_sockopt(&nfsockopt);
	printk(KERN_ALERT "netfilter example 2 clean successfully\n");

}

module_init(nf_sockopt_init);
module_exit(nf_sockopt_exit);

MODULE_AUTHOR("xc");
MODULE_DESCRIPTION("netfilter demo");
MODULE_VERSION("0.0.1");
MODULE_ALIAS("ex 17.2");




