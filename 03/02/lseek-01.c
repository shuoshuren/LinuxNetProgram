#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void){

    off_t offset = -1;

    offset = lseek(stdin,0,SEEK_CUR);
    
    if(-1 == offset){
        printf("SETIN can't seek \n");
        return -1;
    }else{
        printf("SETIN can seek\n");
    }


    return 0;

}
