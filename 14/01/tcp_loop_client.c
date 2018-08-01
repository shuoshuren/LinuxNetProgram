#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFLEN 1024
#define SERVER_PORT 8888

int main(int argc,char *argv[]){
	int s;
	struct sockaddr_in server;
	time_t now;
	char buff[BUFFLEN];
	int n =0;
	int len = 0;

	s = socket(AF_INET,SOCK_STREAM,0);

	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);

	int err = connect(s,(struct sockaddr *)&server,sizeof(server));
	memset(buff,0,BUFFLEN);
	strcpy(buff,"TIME");

	send(s,buff,strlen(buff),0);
	memset(buff,0,BUFFLEN);

	len = sizeof(server);
	n = recv(s,buff,BUFFLEN,0);
	if(n>0){
		printf("Time:%s\n",buff );
	}
	close(s);

	return 0;
}
