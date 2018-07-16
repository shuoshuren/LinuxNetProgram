#include <stdio.h>
#include "add/add.h"
#include "sub/sub.h"


int main(void){
    int a = 10,b=12;
    float x = 6.54321f;
    float y=1.23456f;

    printf("int a+b is:%d\n",add_int(a,b));
    printf("int a-b is:%d\n",sub_int(a,b));
    printf("float x+y is:%f\n",add_float(x,y));
    printf("float x-y is:%f\n",sub_float(x,y));
}
