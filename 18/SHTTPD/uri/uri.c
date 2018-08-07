//
// Created by xc on 18-8-6.
//

#include <ctype.h>
#include "uri.h"



/**
 * 进行字符串转换
 * @param src
 * @param src_len
 * @param dest
 * @param dest_len
 * @return
 */
static int uri_decode(char *src,int src_len,char *dest,int dest_len){
    int i,j,a,b;
#define HEXTOI(x) (isdigit(x)?x-'0':x-'W')
    for (i=j=0;i<src_len && j<dest_len -1;i++,j++){
        switch (src[i]){
            case '%':
                if(isxdigit(((unsigned char *)src)[i+1]) && isxdigit(((unsigned char *)src)[i+2])){
                    a = tolower(((unsigned char *)src)[i+1]);
                    b = tolower(((unsigned char *)src)[i+2]);
                    dest[j] = (HEXTOI(a)<<4) | HEXTOI(b);
                    i+=2;
                }else{
                    dest[j] = '%';
                }
                break;
            default:
                dest[j] = src[i];
                break;
        }
    }
    dest[j] = '\0';
    return (j);
}

/**
 * 对目录中的“..”进行转换
 * @param s
 */
static void remove_double_dots(char *s){
    char *p =s;
    while (*s != '\0'){
        *p++ = *s++;
        if(s[-1] == '/' || s[-1] == '\\'){
            while (*s == '.' || *s == '/' || *s == '\\'){
                s++;
            }
        }
    }
    *p = '\0';
}
