#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#define ADDRLEN 16

int main(int argc,char *argv){
    struct in_addr ip;
    char ip_str[] = "192.168.1.1";
    char addr[ADDRLEN];
    const char* str= NULL;
    int err = 0;

    //测试inet_pton
    err = inet_pton(AF_INET,ip_str,&ip);
    if(err >0){
        printf("inet_pton:ip,%s value is :0x%x\n",ip_str,ip.s_addr);
    }

    //测试inet_ntop
    ip.s_addr = htonl(192<<24|168<<16|12<<8|255);
    str = (const char*) inet_ntop(AF_INET,(void *)&ip,(char *)&addr[0],ADDRLEN);
    if(str){
        printf("inet_ntop:ip,0x%x is %s\n",ip.s_addr,str);
    }

    return 0;
}
