#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>


int main(int argc,char *argv[]){

    char host[] = "wwww.baidu.com";
    struct hostent *ht = NULL;

    ht = gethostbyname(host);

    if(ht){
        int i = 0;
        printf("get the host:%s addr\n",host);
        printf("name:%s\n",ht->h_name);

        printf("type:%s\n",ht->h_addrtype == AF_INET?"AF_INET":"AF_INET6");
        printf("lenght:%d\n",ht->h_length);
        //打印IP
        for(i=0;;i++){
            char * addr = ht->h_addr_list[i];
            if(addr != NULL){
                struct in_addr in=*(struct in_addr *)addr;
                printf("IP: %s\n",inet_ntoa(in));

            }else{
                break;
            }
        }

        //打印域名地址
        for(i=0;;i++){
            if(ht->h_aliases[i] != NULL){
                printf("alias: %s\n",ht->h_aliases[i]);

            }else{
                break;
            }
        }

    }

    return 0;

}
