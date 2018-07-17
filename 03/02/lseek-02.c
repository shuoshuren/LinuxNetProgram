#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*构建空洞文件*/
int main(void){

    int fd = -1,i;
    ssize_t size = -1;
    off_t offset = -1;
    char buf1[] = "01234567";
    char buf2[] = "ABCDEFGH";

    char filename[] = "hole.txt";
    int len = 8;
    
    fd = open(filename,O_RDWR|O_CREAT,S_IRWXU);//创建文件
    if(-1 == fd){
        return -1;
    }

    size = write(fd,buf1,len);
    if(size != len){
        return -1;
    }

    offset = lseek(fd,32,SEEK_SET);//设置文件偏移量为绝对值的32
    if( -1 == offset){
        return -1;
    }

    size = write(fd,buf2,len);
    if(size != len){
        return -1;
    }

    close(fd);

    return 0;

}
