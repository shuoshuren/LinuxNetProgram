#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define BUFFLEN 1024
#define SERVER_PORT 8888
#define BACKLOG 5
#define CLIENTNUM 1024

int connect_host[CLIENTNUM];
int connect_number = 0;

/**
*
* io复用的tcp循环服务器
*
*/


static void * handle_request(void *argv) {
	time_t now;
	char buff[BUFFLEN];
	int n = 0;

	int maxfd = -1;
	fd_set scanfd;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	int err = -1;
	for (;;) {
		maxfd = -1;
		FD_ZERO(&scanfd);
		//将文件描述符放入集合中
		for (int i = 0; i < CLIENTNUM; ++i)
		{
			if (connect_host[i] != -1) {
				FD_SET(connect_host[i], &scanfd);
				if (maxfd < connect_host[i]) {
					maxfd = connect_host[i];
				}
			}
		}
	}

	/*select 等待*/
	err = select(maxfd + 1, &scanfd, NULL, NULL, &timeout);
	switch (err) {
	case 0:
	case -1:
		break;
	default:
		if (connect_number <= 0) {
			break;
		}
		for (int i = 0; i < CLIENTNUM; ++i)
		{
			/* 查找激活的文件描述符 */
			if (connect_host[i] != -1) {
				if (FD_ISSET(connect_host[i], &scanfd)) {
					memset(buff, 0, BUFFLEN);
					n = recv(connect_host[i], buff, BUFFLEN, 0);
					//判断数据是否合法
					if (n > 0 && !strncmp(buff, "TIME", 4)) {
						memset(buff, 0, BUFFLEN);
						now = time(NULL);
						sprintf(buff, "%24s\r\n", ctime(&now));
						send(connect_host[i], buff, strlen(buff), 0);
					}
					//更新文件描述符在数组中的值
					connect_host[i] = -1;
					connect_number --;
					close(connect_host[i]);
				}
			}
		}
		break;

	}

	return NULL;
}



static void * handle_connect(void *argv) {
	int s_s = *((int *)argv);
	int s_c = -1;
	struct sockaddr_in from;
	int len = sizeof(from);

	//接收客户端连接
	for(;;){
		int i = 0;
		int s_c = accept(s_s,(struct sockaddr *)&from,&len);
		printf("a client connect,from:%s\n",inet_ntoa(from.sin_addr));
	
		for (int i = 0; i < CLIENTNUM; ++i)
		{
			if(connect_host[i] == -1){//找到
				//放入
				connect_host[i] = s_c;
				//客户端计数++
				connect_number ++;
				break;
			}
		}
	}	

	return NULL;

}


int main(int argc, char const *argv[])
{
	int s_s;
	struct sockaddr_in local;
	int i = 0;
	memset(connect_host, -1, CLIENTNUM);

	s_s = socket(AF_INET, SOCK_STREAM, 0);

	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);

	int err = bind(s_s, (struct sockaddr *)&local, sizeof(local));
	err = listen(s_s, BACKLOG);

	pthread_t thread_to[2];
	//创建线程处理连接
	pthread_create(&thread_to[0], NULL, handle_connect, (void*)&s_s);

	//创建线程处理请求
	pthread_create(&thread_to[1], NULL, handle_request, NULL);

	//等待线程结束
	for (int i = 0; i < 2; ++i)
	{
		pthread_join(thread_to[i], NULL);
	}


	close(s_s);

	return 0;
}