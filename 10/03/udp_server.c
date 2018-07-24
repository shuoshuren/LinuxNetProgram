#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT_SERV 8888
#define BUFF_LEN 256

static void udpserv_echo(int s,struct sockaddr* client);

int main(int argc,char *argv[]){

    int s;
    struct sockaddr_in addr_server,addr_client;

    s = socket(AF_INET,SOCK_DGRAM,0);

    memset(&addr_server,0,sizeof(addr_server));

    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_server.sin_port = htons(PORT_SERV);

    bind(s,(struct sockaddr*)&addr_server,sizeof(addr_server));

    udpserv_echo(s,(struct sockaddr*)&addr_client);

    return 0;
}

static void udpserv_echo(int s,struct sockaddr *client){
    int n;
    char buff[BUFF_LEN];
    int len;
    while(1){
        len = sizeof(*client);
        n = recvfrom(s,buff,BUFF_LEN,0,client,&len);
        sendto(s,buff,n,0,client,len);
    }
}
