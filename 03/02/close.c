#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void){

    int fd = 0;
    char filename[] = "text.txt";
    int i = 0;
    for (i=0;fd>=0;i++){//循环打开文件直到出错
        fd = open(filename,O_RDONLY);//以只读的方式打开文件
        if(fd == -1){
            printf("Open file %s failed!\n",filename);
        }else{
            printf("fd:%d\n",fd);
        }
    }
    return 0;

}
