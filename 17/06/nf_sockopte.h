#ifndef __SOCKOPTE_H__
#define __SOCKOPTE_H__ 
/* cmd命令：
SOE_BANDIP:ip地址发送禁止命令
SOE_BANDPORT:端口禁止命令
SOE_BANDPING:ping禁止
*/
#define SOE_BANDIP 0x6001
#define SOE_BANDPORT 0x6002
#define SOE_BANDPING 0x6003

/*禁止端口结构*/
typedef struct nf_bandport
{
	unsigned short protocol;
	unsigned short port;

}nf_bandport;

/*与用户交互的数据结构*/
typedef struct band_status{
	unsigned int band_ip;
	nf_bandport band_port;
	unsigned char band_ping;
}band_status;
#endif