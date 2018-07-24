#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT_SERV 8888
#define BUFF_LEN 256
#define NUM_DATA 100
#define LENGTH 1024

static char buff_send[LENGTH];

static void udpclie_echo(int s,struct sockaddr* to);

int main(int argc,char *argv[]){

    int s;
    struct sockaddr_in addr_server,addr_client;

    s = socket(AF_INET,SOCK_DGRAM,0);

    memset(&addr_server,0,sizeof(addr_server));

    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_server.sin_port = htons(PORT_SERV);


    udpclie_echo(s,(struct sockaddr*)&addr_server);

    close(s);

    return 0;
}

static void udpclie_echo(int s,struct sockaddr *to){
    char buff_init[BUFF_LEN] = "UDP TEST";
    struct sockaddr_in from;
    int len = sizeof(*to);
    int i = 0;
    for(i =0;i<NUM_DATA;i++){
        *((int *)&buff_send[0]) = htonl(i);
        memcpy(&buff_send[4],buff_init,sizeof(buff_init));
        sendto(s,&buff_send[0],NUM_DATA,0,to,len);
    }

}
