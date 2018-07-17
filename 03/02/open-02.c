#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void){

    int fd = -1;
    char filename[] = "text.txt";
    fd = open(filename,O_RDWR|O_CREAT|O_EXCL,S_IRWXU);//打开文件，如果文件不纯在，则报错
    if(fd == -1){//文件已经存在
        printf("File %s exist\n",filename);
        fd = open(filename,O_RDWR);//重新打开
        printf("fd:%d\n",fd);
    }else{//文件不存在，创建并打开
        printf("Open file %s success,fd:%d\n",filename,fd);
    }
    return 0;

}
