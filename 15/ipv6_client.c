#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_LEN 1024
#define MYPORT 8888
#define BACKLOG 10

int main(int argc, char const *argv[])
{

	int s_c;
	socklen_t len;
	int err = -1;

	struct sockaddr_in6 server_addr;
	struct sockaddr_in6 client_addr;
	char buf[BUF_LEN + 1];

	//建立IPV6套接字
	s_c = socket(PF_INET6, SOCK_STREAM, 0);
	if (s_c == -1) {
		perror("socket error");
		return 1;
	} else {
		printf("socket() success\n");
	}

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin6_family = PF_INET6;
	server_addr.sin6_port = htons(MYPORT);
	server_addr.sin6_addr = in6addr_any;
	//连接服务器
	err = connect(s_c, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in6));
	if (err == -1) {
		perror("connect error");
		return 1;
	} else {
		printf("connect() success\n");
	}

	//清零缓冲区
	bzero(buf, BUF_LEN + 1);
	//接收客户端消息
	len = recv(s_c, buf, BUF_LEN, 0);
	/*打印消息*/
	printf("Recv %dbytes:%s\n",len,buf );
	bzero(buf,BUF_LEN+1);
	strcpy(buf,"From client");
	len = send(s_c,buf,strlen(buf),0);
	close(s_c);


	return 0;
}