//
// Created by xc on 18-8-3.
//
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include "config.h"

/**
 * 参数初始化
 * @param argc
 * @param argv
 */
 void para_init(int argc,char *argv[]){
    para_cmd_parse(argc,argv);
}

/**
* 命令行解析
* @param argc
* @param argv
* @return
*/
static int para_cmd_parse(int argc, char *argv[]) {
    int c;
    size_t len;
    long value;
    //遍历输入参数，设置配置参数
    while ((c = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1) {
        switch (c) {
            case 'c'://CGI跟路径
                l_opt_arg = optarg;
                if (l_opt_arg && l_opt_arg[0] != ':') {
                    len = strlen(l_opt_arg);
                    memcpy(conf_para.CGIRoot, l_opt_arg, len + 1);
                }
                break;
            case 'd'://默认文件名称
                l_opt_arg = optarg;
                if (l_opt_arg && l_opt_arg[0] != ':') {
                    len = strlen(l_opt_arg);
                    memcpy(conf_para.DefaultFile, l_opt_arg, len + 1);
                }
                break;
            case 'f'://配置文件路径
                l_opt_arg = optarg;
                if (l_opt_arg && l_opt_arg[0] != ':') {
                    len = strlen(l_opt_arg);
                    memcpy(conf_para.ConfigFile, l_opt_arg, len + 1);
                }
                break;
            case 'o'://根文件
                l_opt_arg = optarg;
                if (l_opt_arg && l_opt_arg[0] != ':') {
                    len = strlen(l_opt_arg);
                    memcpy(conf_para.DocumentRoot, l_opt_arg, len + 1);
                }
                break;
            case 'l'://监听端口
                l_opt_arg = optarg;
                if (l_opt_arg && l_opt_arg[0] != ':') {
                    len = strlen(l_opt_arg);
                    value = strtol(l_opt_arg, NULL, 10);
                    if (value != LONG_MAX && value != LONG_MIN) {
                        conf_para.ListenPort = value;
                    }
                }
                break;
            case 'm'://最大客户端
                l_opt_arg = optarg;
                if (l_opt_arg && l_opt_arg[0] != ':') {
                    len = strlen(l_opt_arg);
                    value = strtol(l_opt_arg, NULL, 10);
                    if (value != LONG_MAX && value != LONG_MIN) {
                        conf_para.MaxClient = value;
                    }
                }
                break;
            case 't'://超时时间
                l_opt_arg = optarg;
                if (l_opt_arg && l_opt_arg[0] != ':') {
                    len = strlen(l_opt_arg);
                    value = strtol(l_opt_arg, NULL, 10);
                    if (value != LONG_MAX && value != LONG_MIN) {
                        conf_para.TimeOut = value;
                    }
                }
                break;
            case '?':
                printf("Invalid para\n");
                break;
            case 'h'://帮助
                display_usage();
                break;

        }
    }
    return 0;
}

/**
 * 解析配置文件
 * @param file
 */
static void para_file_parse(char *file) {
#define LINELENGTH 256
    char line[LINELENGTH];
    char *name = NULL, *value = NULL;
    int fd = -1;
    int n = 0;
    fd = open(file, O_RDONLY);
    if (fd == -1) {
        printf("%s 打开失败\n", file);
        goto EXITpara_file_parse;
    }
    /**
     * 命令格式如下：
     * #注释|[空格]关键字[空格]=[空格]value
     */
    while ((n = conf_readline(fd, line, LINELENGTH)) != 0) {
        char *pos = line;
        while (isspace(*pos)) {//跳过一行开头部分的空格
            pos++;
        }
        if (*pos == '#') {//跳过注释
            continue;
        }

        name = pos; //关键字开始的部分
        while (!isspace(*pos) && *pos != '=') {//关键字末尾
            pos++;
        }
        *pos = '\0';//生成关键字字符串

        while (isspace(*pos)) {//value 前面的空格
            pos++;
        }

        value = pos;//value开始
        while (!isspace(*pos) && *pos != '\r' && *pos != '\n') {
            pos++;
        }
        *pos = '\0';//生成value字符串

        int ivalue;
        if (strncmp("CGIRoot", name, 7)) {//CGIRoot部分
            memcpy(conf_para.CGIRoot, value, strlen(value) + 1);
        } else if (strncmp("DefaultFile", name, 11)) {
            memcpy(conf_para.DefaultFile, value, strlen(value) + 1);
        } else if (strncmp("DocumentRoot", name, 12)) {
            memcpy(conf_para.DocumentRoot, value, strlen(value) + 1);
        } else if (strncmp("ListenPort", name, 10)) {
            ivalue = strtol(value, NULL, 10);
            conf_para.ListenPort = ivalue;
        } else if (strncmp("MaxClient", name, 9)) {
            ivalue = strtol(value, NULL, 10);
            conf_para.MaxClient = ivalue;
        } else if (strncmp("TimeOut", name, 7)) {
            ivalue = strtol(value, NULL, 10);
            conf_para.TimeOut = ivalue;
        }

    }

    close(fd);


    EXITpara_file_parse:
    return;
}

/**
 * 文件读取一行
 * @param fd
 * @param buff
 * @param len
 * @return
 */
static size_t conf_readline(int fd, char *buff, int len) {
    size_t count = -1;
    while(fgets(buff,len,fd)!=NULL){
        count = strlen(buff);
        buff[count-1] = '\0';
        printf("line:%s\n",buff);
    }
    return count;
}

/**
 * 打印帮助信息
 */
static void display_usage() {
    printf("help\n");
}

