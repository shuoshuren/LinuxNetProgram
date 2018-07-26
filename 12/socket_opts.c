#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define PORT 8888
#define BACKLOG 8

static int alive = 1;


/* handle sigpip and sigint signal*/
static int sigpipe(int signo){
	alive = 0;
}

int main(int argc, char const *argv[])
{
	
	int s,sc;//s server_socket,sc client_socket
	struct sockaddr_in local_addr,client_addr;
	int err = -1;
	socklen_t optlen = -1;
	int optval = -1;
	signal(SIGPIPE,sigpipe);
	signal(SIGINT,sigpipe);

	//create socket
	s = socket(AF_INET,SOCK_STREAM,0);
	if(s == -1){
		printf("create socket error\n");
		return -1;
	}
	//set ip and port reuse 
	optval = 1;
	optlen = sizeof(optval);
	err = setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *)&optval,optlen);
	if(err != -1){
		printf("socket reuse setting failed\n");
		return -1;
	}

	//init ip and port
	bzero(&local_addr,0);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;

	//bind socket
	err = bind(s, (struct sockaddr *) &local_addr,sizeof(struct sockaddr));
	if(err == -1){
		printf("bind failed\n");
		return -1;
	}

	//set max recv buffer and send buffer
	optval = 128*1024;
	optlen = sizeof(optval);
	err = setsockopt(s,SOL_SOCKET,SO_RCVBUF,&optval,optlen);
	if(err == -1){
		printf("set recv buffer failed\n");
	}
	err = setsockopt(s,SOL_SOCKET,SO_SNDBUF,&optval,optlen);
	if(err == -1){
		printf("set send buffer failed\n");
	}

	//set recv and send outtime
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 200*1000;
	optlen = sizeof(tv);
	err = setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tv,optlen);
	if(err == -1){
		printf("set recv outtime failed\n");
	}
	err = setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,&tv,optlen);
	if(err == -1){
		printf("set send outtime failed\n");
	}

	//set listener
	err = listen(s,BACKLOG);
	if(err == -1){

		printf("set listen failed\n");
		return -1;
	}

	//set accept outtime
	printf("wait connect...\n");
	fd_set fd_r;
	// struct timeval tv;
	tv.tv_usec = 200*1000;
	tv.tv_sec = 0;
	while(alive){
		socklen_t sin_size = sizeof(struct sockaddr_in);
		//select 
		FD_ZERO(&fd_r);
		FD_SET(s,&fd_r);
		switch(select(s,(fd_set *)1,&fd_r,NULL,&tv)){
			case -1:
			case 0:
				continue;
				break;
			default:
				break;
		}
		sc = accept(s,(struct sockaddr *)&client_addr,&sin_size);
		if(sc == -1){
			perror("accept connect failed\n");
			continue;
		}

		//set connect outtime
		optval = 10;
		optlen = sizeof(optval);
		err = setsockopt(sc,IPPROTO_TCP,SO_KEEPALIVE,(char *)&optval,optlen);
		if(err == -1){
			printf("set connect outtime failed\n");
		}

		//turn off Nagle
		optval = 1;
		optlen = sizeof(optval);
		err = setsockopt(sc,IPPROTO_TCP,TCP_NODELAY,(char *)&optval,optlen);
		if(err == -1){
			printf("turn off Nagle failed\n");
		}

		//set linger
		struct linger linger;
		linger.l_onoff = 1;
		linger.l_linger = 0;
		optlen = sizeof(linger);
		err = setsockopt(sc,SOL_SOCKET,SO_LINGER,(char*)&linger,optlen);
		if(err == -1){
			printf("set linger failed\n");
		}

		//printf client socket
		printf("recv %s connect\n",inet_ntoa(client_addr.sin_addr) );
		err = send(sc,"connect success!\n",17,0);
		if(err == -1){
			printf("send msg failed\n");
		}

		close(sc);
	}

	close(s);

	return 0;
}

