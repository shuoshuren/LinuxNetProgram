#include <sys/types.h>
#include <sys/socket.h>
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
#define CLIENTNUM 2

pthread_mutex_t ALOCK = PTHREAD_MUTEX_INITIALIZER;

/**
*
* 单客户端单线程，各线程独自accept()，使用互斥锁
*
*/


static void handle_request(void *argv) {
	int s_s = *((int *)argv);
	int s_c;
	struct sockaddr_in from;
	int len = sizeof(from);
	for (;;) {
		time_t now;
		char buff[BUFFLEN];
		int n = 0;

		pthread_mutex_lock(&ALOCK);
		s_c = accept(s_s, (struct sockaddr*)&from, &len);
		pthread_mutex_unlock(&ALOCK);

		memset(buff, 0, BUFFLEN);
		n = recv(s_c, buff, BUFFLEN, 0);

		if (n > 0 && !strncmp(buff, "TIME", 4)) {
			memset(buff, 0, BUFFLEN);
			now = time(NULL);
			sprintf(buff, "%24s\r\n", ctime(&now));
			send(s_c, buff, strlen(buff), 0);
		}
		close(s_c);
	}


}

static void handle_connect(int s) {
	int s_s = s;
	pthread_t thread_to[CLIENTNUM];
	for (int i = 0; i < CLIENTNUM; ++i) {
		//创建线程处理连接
		int err = pthread_create(&thread_to[i], NULL, handle_request, (void*)&s_s);
	}
	//等待线程结束
	for (int i = 0; i < CLIENTNUM; ++i)
	{
		pthread_join(thread_to[i], NULL);
	}

}

void sig_int(int num) {
	exit(1);
}

int main(int argc, char const *argv[])
{
	int s_s;
	struct sockaddr_in local;

	signal(SIGINT, sig_int);

	s_s = socket(AF_INET, SOCK_STREAM, 0);

	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(SERVER_PORT);

	int err = bind(s_s, (struct sockaddr *)&local, sizeof(local));
	err = listen(s_s, BACKLOG);

	handle_connect(s_s);

	close(s_s);

	return 0;
}