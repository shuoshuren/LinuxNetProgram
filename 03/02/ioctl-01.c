#include <linux/cdrom.h>
#include <stdio.h>
#include <fcntl.h>

int main(void){
    
    int fd = open("/dev/cdrom",O_RDONLY);//打开CDRom设备文件
    if(fd <0){
        printf("打开CDROM失败\n");
        return -1;
    }
    //向Linux内核CDRom 驱动程序发送CDROMEJECT请求

    if(!ioctl(fd,CDROMEJECT,NULL)){
        printf("成功弹出COROM\n");
    }else{
        printf("弹出失败\n");
    }

    close(fd);
    return 0;
}
