#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/un.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

static void display_err(const char* on_what){
    perror(on_what);
    exit(1);
}

int main(int argc,char* argv[]){
    int error;
    int sock_unix;
    struct sockaddr_un addr_unix;
    int len_unix;
    const char path[]="/demon/path";

    //建立套接字
    sock_unix = socket(AF_UNIX,SOCK_STREAM,0);

    if(sock_unix == -1){
        display_err("socket()");
    }

    unlink(path);

    //填充地址结构
    memset(&addr_unix,0,sizeof(addr_unix));

    addr_unix.sun_family = AF_LOCAL;
    strcpy(addr_unix.sun_path,path);
    len_unix = sizeof(struct sockaddr_un);

    //bind
    error = bind(sock_unix,(struct sockaddr*)&addr_unix,len_unix);
    if(error == -1){
        display_err("bind()");
    }

    //close
    close(sock_unix);
    unlink(path);

    return 0;
}
