#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>

int main(int argc, char const *argv[])
{
	
	int err = -1;
	int s = -1;
	int snd_size = 0;
	int rcv_size = 0;
	socklen_t optlen;

	/*tcp socket*/
	s = socket(PF_INET,SOCK_STREAM,0);
	if(s == -1){
		printf("create socket error\n");
		return -1;
	}

	/*read send buffer size*/
	optlen = sizeof(snd_size);
	err = getsockopt(s,SOL_SOCKET,SO_SNDBUF,&snd_size,&optlen);
	if(err){
		printf("get send buffer size error\n");
	}

	/*pintf buffer size*/
	printf("send buffer size:%d byte\n",snd_size );
	printf("recv buffer size:%d byte\n",rcv_size );

	/*read recv buffer size*/
	optlen = sizeof(rcv_size);
	err = getsockopt(s,SOL_SOCKET,SO_RCVBUF,&rcv_size,&optlen);
	if(err){
		printf("get recv buffer size error\n");
	}

	/*set send buffer size*/

	snd_size = 4096;
	optlen = sizeof(snd_size);
	err = setsockopt(s,SOL_SOCKET,SO_SNDBUF,&snd_size,optlen);
	if(err){
		printf("set send buffer size error\n");
	}

	/*set recv buffer size*/
	rcv_size = 8192;
	optlen = sizeof(rcv_size);
	err = setsockopt(s,SOL_SOCKET,SO_RCVBUF,&rcv_size,optlen);
	if(err){
		printf("set recv buffer size error\n");
	}

	/*get change send buffer size*/
	optlen = sizeof(snd_size);
	err = getsockopt(s,SOL_SOCKET,SO_SNDBUF,&snd_size,&optlen);
	if(err){
		printf("get send buffer size error\n");
	}

	/*get change recv buffer size*/
	optlen = sizeof(rcv_size);
	err = getsockopt(s,SOL_SOCKET,SO_RCVBUF,&rcv_size,&optlen);
	if(err){
		printf("get recv buffer size error\n");
	}

	/*pintf buffer size*/
	printf("send buffer size:%d byte\n",snd_size );
	printf("recv buffer size:%d byte\n",rcv_size );

	close(s);

	return 0;
}