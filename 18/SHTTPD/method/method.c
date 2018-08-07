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
    printf("n:%ld\n",n);
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
            "HTTP/1.1 %ld %s\r\n"
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

/**
 * 根据输入的扩展名查找内容类型的匹配项
 * @param uri
 * @param len
 * @param wctl
 * @return
 */
struct mine_type * Mine_Type(char *uri,size_t len,struct worker_ctl *wctl){

    printf("==> Mine_Type\n");
    int i=0;
    char *ext = memcpy(uri,'.',len);//查找扩展名的位置
    struct mine_type *mine = NULL;
    int found = 0;
    ext++;
    printf("uri:%s,len:%d,ext is %d,%s\n",uri,len,ext,ext);
    for(mine = &builtin_mine_types[i];mine->extension != NULL;i++){
        if(!strncmp(mine->extension,ext,mine->ext_len)){
            found = 1;
            printf("found it,ext is %s\n",mine->extension);
            break;
        }
    }
    if(!found){
        mine = &builtin_mine_types[2];
    }
    printf("<== Mine_Type\n");

    return mine;
}

/**
 * 生成错误类型
 * @param wctl
 * @return
 */
int generate_error_mine(struct worker_ctl * wctl){
    struct error_mine *err = NULL;
    int i=0;
    for(err = &_error_http[i];err->error_code!=wctl->conn.con_res.status;i++){
        ;
    }
    if(err->error_code!=wctl->conn.con_res.status){
        err = &_error_http[0];
    }
    snprintf(
            wctl->conn.dres,
            sizeof(wctl->conn.dres),
            "HTTP/%lu.%lu %d %s\r\n"
            "Content-Type:%s\r\n"
            "Content-Length:%d\r\n"
            "\r\n"
            "%s",
            wctl->conn.con_req.major,
            wctl->conn.con_req.minor,
            err->error_code,
            err->msg,
            "text/plain",
            strlen(err->content),
            err->content);
    wctl->conn.con_res.cl = strlen(err->content);
    wctl->conn.con_res.fd = -1;
    wctl->conn.con_res.status = err->error_code;
}