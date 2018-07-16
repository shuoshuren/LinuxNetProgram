/*动态加载库示例*/

#include <dlfcn.h>/*动态加载库库头*/
#include <stdio.h>
#include <stdlib.h>

int main(void){
    char src[] = "Hello Dymatic";//要计算的字符串
    int (*pStrLenFun) (char *str);//库函数指针
    void *phandle = NULL;//库句柄
    char *perr=NULL;//错误信息指针
    phandle = dlopen("./libstr.so",RTLD_LAZY);//打开动态链接库

    if(!phandle){
        printf("Filed load library!\n");
    }

    perr = dlerror();//读取错误信息
    if(perr != NULL){
        printf("%s\n",perr);//打印错误
        return 0;
    }

    pStrLenFun = dlsym(phandle,"str_len");//获取函数str_len地址
    perr = dlerror();
    if(perr != NULL){
        printf("%s\n",perr);
        return 0;
    }

    printf("the string length is:%d\n",pStrLenFun(src));
    dlclose(phandle);//关闭动态链接库
    return 0;
}


