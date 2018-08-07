//
// Created by xc on 18-8-6.
//


#ifndef SHTTPD_METHOD_H
#define SHTTPD_METHOD_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../base/base.h"

#define big_int_t long int

extern vec vec_shttpd_methods[] = {
    { "GET", 3, METHOD_GET },
    { "POST", 4, METHOD_POST },
    { "PUT", 3, METHOD_PUT },
    { "DELETE", 6, METHOD_DELETE },
    { "HEAD", 4, METHOD_HEAD },
    { NULL, 0 }
};

struct mine_type{
    char *mime_type;
};


/**
 * 方法的总函数
 * @param wctl
 */
void method_do(struct worker_ctl *wctl);

void method_do_cgi(struct worker_ctl *wctl);

void method_put(struct worker_ctl *wctl);

void method_delete(struct worker_ctl *wctl);

int method_get(struct worker_ctl *wctl);

void method_post(struct worker_ctl *wctl);

void method_head(struct worker_ctl *wctl);

void method_list(struct worker_ctl *wctl);

struct mine_type * Mine_Type(char *uri,size_t len,struct worker_ctl *wctl);

#endif //SHTTPD_METHOD_H
