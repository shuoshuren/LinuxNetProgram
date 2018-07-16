#include<stdio.h>

extern int str_len(char* str);

int main(void){
    char src[] = "Hello Dymatic";
    printf("String %s length is:%d\n",src,str_len(src));
    return 0;
}
