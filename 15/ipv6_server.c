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
	
	int s_s,s_c;
	socklen_t len;
	int err = -1;

	struct sockaddr_in6 local_addr;
	struct sockaddr_in6 client_addr;
	char buf[BUF_LEN+1];

	//建立IPV6套接字
	s_s = socket(PF_INET6,SOCK_STREAM,0);
	if(s_s == -1){
		perror("socket error");
		return 1;
	}else{
		printf("socket() success\n");
	}

	bzero(&local_addr,sizeof(local_addr));
	local_addr.sin6_family = PF_INET6;
	local_addr.sin6_port = htons(MYPORT);
	local_addr.sin6_addr = in6addr_any;
	err = bind(s_s,(struct sockaddr *)&local_addr,sizeof(struct sockaddr_in6));
	if(err == -1){
		perror("bind error");
		return 1;
	}else{
		printf("bind() success\n");
	}

	err = listen(s_s,BACKLOG);
	if(err == -1){
		perror("listen error");
		exit(1);
	}else{
		printf("listen() success\n");
	}

	while(1){
		len = sizeof(struct sockaddr);
		s_c = accept(s_s,(struct sockaddr *)&client_addr,&len);
		if(s_c == -1){
			perror("accept error");
			return (errno);
		}else{
			//将客户端地址转换为字符串
			inet_ntop(AF_INET6,&client_addr.sin6_addr,buf,sizeof(buf));
			printf("a client from ip:%s,port %d,socket %d\n",buf,client_addr.sin6_port,s_c );
		}
		//处理新连接的数据收发
		bzero(buf,BUF_LEN+1);
		strcpy(buf,"From Server");
		//发送消息到客户端
		len = send(s_c,buf,strlen(buf),0);
		if(len <0){
			printf("message '%s' send error,errno:%d,'%s'\n",buf,errno,strerror(errno) );		
		}else{
			printf("message '%s' send success,%d bytes\n", buf,len);
		}
		//清零缓冲区
		bzero(buf,BUF_LEN+1);
		//接收客户端消息
		len = recv(s_c,buf,BUF_LEN,0);
		if(len >0){
			printf("recv message success:'%s',%dbytes\n",buf,len);
		}else{
			printf("recv message failure,errno:%d,'%s'\n",errno,strerror(errno) );
		}

		close(s_c);

	}

	return 0;
}