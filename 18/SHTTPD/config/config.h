//
// Created by xc on 18-8-3.
//
#include <getopt.h>
#include <zconf.h>

#ifndef SHTTPD_CONFIG_H
#define SHTTPD_CONFIG_H

struct conf_opts{
    char    CGIRoot[128];       //CGI根路径
    char    DefaultFile[128];   //默认文件名称
    char    DocumentRoot[128];  //根文件路径
    char    ConfigFile[128];    //配置文件路径和名称
    int     ListenPort;         //监听端口
    int     MaxClient;          //最大客户端数量
    int     TimeOut;            //超时时间
    int     InitClient;         //初始化线程数量
};

struct conf_opts conf_para = {
        /*CGIRoot*/         "/usr/local/var/www/cgi-bin",
        /*DefaultFile*/     "index.html",
        /*DocumentRoot*/    "/usr/local/var/www",
        /*ConfigFile*/      "/etc/SHTTPD.conf",
        /*ListenPort*/      8080,
        /*MaxClient*/       4,
        /*TimeOut*/         3,
        /*InitClient*/      2
};
//短选项配置
static char *shortopts = "c:d:f:ho:l:m:t:";

//长选项配置
static struct option longopts[]={
        {"CGIRoot",required_argument,NULL,'c'},
        {"DefaultFile",required_argument,NULL,'d'},
        {"ConfigFile",required_argument,NULL,'f'},
        {"DocumentRoot",required_argument,NULL,'o'},
        {"ListenPort",required_argument,NULL,'l'},
        {"MaxClient",required_argument,NULL,'m'},
        {"TimeOut",required_argument,NULL,'t'},
        {"Help",no_argument,NULL,'h'},
        {0,0,0,0},
};

static char *l_opt_arg;

/**
 * 命令行解析函数
 * @param argc
 * @param argv
 * @return
 */
static int para_cmd_parse(int argc,char *argv[]);

/**
 * 解析文件
 * @param file
 */
static void para_file_parse(char *file);

/**
 * 参数初始化
 * @param argc
 * @param argv
 */
extern void para_init(int argc,char *argv[]);

/**
 * 文件读取一行
 * @param fd
 * @param buff
 * @param len
 * @return
 */
static size_t conf_readline(int fd, char* buff, int len);

/**
 * 打印帮助信息
 */
static void display_usage();

#endif //SHTTPD_CONFIG_H
