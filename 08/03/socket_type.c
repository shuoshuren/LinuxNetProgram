#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

int is_socket_type(int fd){

    struct stat st;
    int err = fstat(fd,&st);

    if(err <0){
        return -1;
    }

    if((st.st_mode & S_IFMT) == S_IFSOCK){
        //是套接字
        return 1;
    }else{
        return 0;
    }
}

int main(void){
    int ret = is_socket_type(0);
    printf("stdin value %d\n",ret);

    int s = socket(AF_INET,SOCK_STREAM,0);
    ret = is_socket_type(s);
    printf("socket value %d\n",ret);
  
    return 0;
}
