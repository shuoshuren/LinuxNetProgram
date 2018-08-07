//
// Created by xc on 18-8-6.
//

#ifndef SHTTPD_BASE_H
#define SHTTPD_BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pthread.h>


#define URI_MAX 256

typedef enum _MethodType {
    METHOD_GET=0,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_HEAD
} SHTTPD_METHOD_TYPE;

typedef struct vec {
    char *ptr;//请求方法名称
    int len;//请求方法长度
    SHTTPD_METHOD_TYPE type;//请求方法类型
} vec;



union variant{
    struct v_vec{
        int len;
        char *ptr;
    }v_vec;

};

struct headers {
    union variant cl;//内容长度
    union variant ct;//内容类型
    union variant connection;//连接状态
    union variant ims;//最后修改时间
    union variant user;//用户名称
    union variant auth;//权限
    union variant useragent;//用户代理
    union variant referer;//参考
    union variant cookie;//Cookie
    union variant location;//位置
    union variant range;//范围
    union variant status;//状态值
    union variant transenc;//编码类型

};

/**
 * 请求结构
 */
struct conn_request {
    struct vec req;//请求向量
    char *head;//请求头
    char *uri;//请求uri
    char rpath[URI_MAX];//请求文件的真实地址
    int method;//请求类型
    unsigned long major;//http 主版本
    unsigned long minor;//http 副版本
    struct headers ch;//头部结构
    struct worker_conn *conn;//连接结构指针
    int err;//错误代码
};



/**
 * 响应结构
 */
struct conn_response {
    struct vec res;//响应向量
    time_t birth_time;//建立时间
    time_t expire_time;//超时时间
    int status;//响应状态值
    int cl;//响应内容长度
    int fd;//请求文件描述符
    struct stat fsate;//请求文件状态
    struct worker_conn *conn;//连接结构指针
};

struct worker_opts {
    pthread_t th;//线程的id号
    int flags;//线程状态
    pthread_mutex_t mutex;//线程任务互斥
    struct worker_ctl *work;//本线程的总控结构
};

struct worker_conn {
#define K 1024
    char dreq[16 * K]; //请求缓冲区
    char dres[16 * K];//响应缓冲区
    int cs;//客户端socket描述符
    int to;//客户端无响应时间超时退出时间
    struct conn_response con_res;//响应结构
    struct conn_request con_req;//请求结构
    struct worker_ctl *work;//本线程的总控结构
};

struct worker_ctl{
    struct worker_opts opts;
    struct worker_conn conn;
};



#endif //SHTTPD_BASE_H
