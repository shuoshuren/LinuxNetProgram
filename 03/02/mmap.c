#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

#define FILE_LEN 80

/*使用mmap对文件进行操作*/
int main(void){

    int fd = -1;
    char buf[] = "quick brown fox jumps over the lazy dog";

    char *ptr = NULL;
    
    fd = open("mmap.txt",O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
    if(-1 == fd){
        return -1;
    }

    lseek(fd,FILE_LEN - 1,SEEK_SET);

    write(fd,'a',10);

    ptr = (char *)mmap(NULL,FILE_LEN,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    if((char *)-1 == ptr){//映射失败，则退出
        printf("mmap failure\n");
        close(fd);
        return -1;
    }

    memcpy(ptr+16,buf,strlen(buf));//将buf中字符串复制到映射区

    munmap(ptr,FILE_LEN);//取消映射

    close(fd);

    return 0;

}
