#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <linux/netfilter.h>
#include <string.h>
#include <errno.h>
#include "nf_sockopte.h"

int main(){

	band_status status;
	socklen_t len;
	int sockfd;
	len = sizeof(status);

	if((sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_RAW)) == -1){
		return -1;
	}

	if(getsockopt(sockfd,IPPROTO_IP,SOE_BANDPING,(char *)&status,&len)){
		return -1;
	}
	return 0;

}