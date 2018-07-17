#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void){

    int fd = -1,i=0;
    ssize_t size = -1;
    char buf[] = "quick brown fox jumps over the lazy dog";
    char filename[] = "text.txt";

    fd = open(filename,O_RDWR);
    if(-1 == fd){
        printf("open file %s failed \n",filename);
    }else{
        printf("open file %s success,fd:%d\n",filename,fd);
    }


    size = write(fd,buf,strlen(buf));
    printf("write %ld bytes to file %s \n",size,filename);
    
    close(fd);

    return 0;

}
