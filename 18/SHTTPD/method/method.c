//
// Created by xc on 18-8-6.
//

#include <memory.h>
#include "method.h"



/**
 * 方法的总函数
 * @param wctl
 */
void method_do(struct worker_ctl *wctl){
    printf("==>method do\n");
    if(0){
        method_do_cgi(wctl);
    }

    switch (wctl->conn.con_req.method){
        case METHOD_PUT:
            method_put(wctl);
            break;
        case METHOD_DELETE:
            method_delete(wctl);
            break;
        case METHOD_GET:
            method_get(wctl);
            break;
        case METHOD_POST:
            method_post(wctl);
            break;
        case METHOD_HEAD:
            method_head(wctl);
            break;
        default:
            method_list(wctl);
    }
    printf("<== method do\n");
}

void method_do_cgi(struct worker_ctl *wctl);

void method_put(struct worker_ctl *wctl);

void method_delete(struct worker_ctl *wctl);

int method_get(struct worker_ctl *wctl){
    printf("==> Method Get\n");
    struct conn_response *res = &wctl->conn.con_res;
    struct conn_request *req = &wctl->conn.con_req;
    char path[URI_MAX];
    memset(path,0,URI_MAX);
    size_t  n;
    unsigned  long r1,r2;
    char *fmt = "%a, %d %b %Y %H:%M:%S GMT";
    //需要确定的参数
    size_t status = 200;
    char *msg = "OK";
    char date[64] = "";
    char lm[64] = "";
    char etag[64] = "";
    big_int_t cl;
    char range[64] = "";
    struct mine_type *mine = NULL;
    time_t t = time(NULL);
    //当前时间
    strftime(date,sizeof(date),fmt,localtime(&t));
    //最后修改时间
    strftime(lm,sizeof(lm),fmt,localtime(&res->fsate.st_mtim.tv_sec));
    //ETAG
    snprintf(etag,sizeof(etag),"%1x.%1x",
             (unsigned int)res->fsate.st_mtim.tv_sec,
            (unsigned int)res->fsate.st_size);
    //发送的MIME类型
    mine = Mine_Type(req->uri,strlen(req->uri),wctl);
    //内容长度
    cl = (big_int_t) res->fsate.st_size;
    //范围
    memset(range,0,sizeof(range));
    n=-1;
    if(req->ch.range.v_vec.len>0){
        printf("request range:%d\n",req->ch.range.v_vec.len);
        n = sscanf(req->ch.range.v_vec.ptr,"bytes=%lu-%lu",&r1,r2);
    }
    printf("n:%d\n",n);
    if(n>0){
        status = 206;
        (void) fseek(res->fd,r1,SEEK_SET);
        cl = n == 2?r2-r1+1:cl-r1;
        snprintf(range,sizeof(range),"Content-Range: bytes %lu-%lu/%lu\r\n",
                r1,r1+cl-1,
                 (unsigned long)res->fsate.st_size);
        msg = "Partial Content";
    }
    //输出构建的头部数据
    memset(res->res.ptr,0,sizeof(wctl->conn.dres));
    snprintf(res->res.ptr,sizeof(wctl->conn.dres),
            "HTTP/1.1 %d %s\r\n"
            "Date: %s\r\n"
            "Last-Modified: %s\r\n"
            "Etag: \"%s\"\r\n"
            "Content-Type: %.*s\r\n"
            "Content-Length: %lu\r\n"
            "Accept-Ranges: bytes\r\n"
            "%s\r\n",
            status,
            msg,
            date,
            lm,
            etag,
            strlen(mine->mime_type),
            mine->mime_type,
            cl,
            range
            );
    res->cl = cl;
    res->status = status;
    printf("<== Method Get\n");
    return 0;
}

void method_post(struct worker_ctl *wctl);

void method_head(struct worker_ctl *wctl);

void method_list(struct worker_ctl *wctl);

struct mine_type * Mine_Type(char *uri,size_t len,struct worker_ctl *wctl){
    
}