#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void){

    struct stat st;
    if(-1 == stat("text.txt",&st)){//获取文件状态
        printf("获取文件状态失败\n");
        return -1;
    }

    printf("dev id:%ld\n",st.st_dev);
    printf("节点:%ld\n",st.st_ino);
    printf("保护模式:%d\n",st.st_mode);
    printf("硬链接数:%ld\n",st.st_nlink);
    printf("owner id:%d\n",st.st_uid);
    printf("gid:%d\n",st.st_gid);
    printf("dev id(特殊设备):%ld\n",st.st_rdev);
    printf("文件大小:%ld\n",st.st_size);
    printf("文件块大小:%ld\n",st.st_blksize);
    printf("占用块数量:%ld\n",st.st_blocks);
    printf("最后访问时间:%ld\n",st.st_atime);
    printf("最后修改时间:%ld\n",st.st_mtime);
    printf("状态改变时间:%ld\n",st.st_ctime);

    return 0;
}
