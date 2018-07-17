#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void){

    int fd = -1,i=0;
    ssize_t size = -1;
    char buf[10];
    char filename[] = "text.txt";

    fd = open(filename,O_RDONLY);
    if(-1 == fd){
        printf("open file %s failed \n",filename);
    }else{
        printf("open file %s success,fd:%d\n",filename,fd);
    }

    while(size){//循环读取数据，直到文件末尾或者出错
        size = read(fd,buf,10);
        if(-1 == size){//读取出错
            close(fd);
            printf("read file error occurs\n");
            return -1;
        }else{
            if(size >0){
                printf("read %d bytes:",size);
                printf("\"");
                for(i = 0;i<size;i++){
                    printf("%c",*(buf+i));
                }
                printf("\"\n");
            }else{
                printf("reach the end of file\n");
            }
        }
    }

    return 0;

}
