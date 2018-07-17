#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void){

    int fd = -1;
    char filename[] = "text.txt";
    fd = open(filename,O_RDWR);//以可读写的方式打开文件
    if(fd == -1){
        printf("Open file %s failed!\n",filename);
    }else{
        printf("Open file %s success,fd:%d\n",filename,fd);
    }
    return 0;

}
