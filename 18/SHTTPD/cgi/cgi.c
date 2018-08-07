//
// Created by xc on 18-8-7.
//

#include <memory.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "cgi.h"
#include "../config/config.h"

int cgi_handler(struct worker_ctl *wctl){
    struct conn_request *req = &wctl->conn.con_req;
    struct conn_response *res = &wctl->conn.con_res;
    char *command = strstr(req->uri,CGISTR) +strlen(CGISTR);//获取匹配字符串/cgi-bin/之后的地址
    char *arg[ARGNUM];
    int num = 0;
    char *rpath = wctl->conn.con_req.rpath;
    struct stat *fs = &wctl->conn.con_res.fsate;
    int retval = -1;

    //生成CGI全路径
    char *pos = command;
    for(;*pos != '?' && *pos != '\0';pos++){
        ;
    }
    *pos = '\0';
    sprintf(rpath,"%s%s",conf_para.CGIRoot,command);
    //获取参数
    pos++;
    for(;*pos != '\0' && num <ARGNUM;){
        arg[num] = pos;
        for(;*pos !='+' && *pos != '\0';pos++){
            ;
        }
        if(*pos == '+'){
            *pos = '\0';
            pos++;
            num++;
        }
    }
    arg[num] = NULL;
    //查看cgi命令的属性
    if(stat(rpath,fs)<0){
        res->status = 403;
        retval = -1;
        return retval;
    }
    else if((fs->st_mode & S_IFDIR) == S_IFDIR){
        //是一个目录
    }else if((fs->st_mode & S_IXUSR) != S_IXUSR){
        //所指文件不能执行
        res->status = 403;
        retval = -1;
        return retval;
    }else{

    }
    //创建管道用于进程间通讯
    int pipe_in[2];
    int pipe_out[2];
    if(pipe(pipe_in) <0){
        res->status = 500;
        retval = -1;
        return retval;
    }
    if(pipe(pipe_out)<0){
        res->status = 500;
        retval = -1;
        return retval;
    }
    //进程分叉
    int pid = 0;
    pid = fork();
    if(pid<0){
        res->status = 500;
        retval = -1;
        return retval;
    }else if(pid>0){//父进程
        close(pipe_out[WRITEOUT]);
        close(pipe_in[READIN]);
        //主进程从CGI输出读取数据，发送到客户端
        int size = 0;
        int end = 0;
        while (size >0 && !end){
            size = read(pipe_out[READIN],res->res.ptr,sizeof(wctl->conn.dres));
            if(size>0){
                send(wctl->conn.cs,res->res.ptr,strlen(res->res.ptr));
            }else{
                end = 1;
            }
        }
        wait(&end);
        close(pipe_out[READIN]);
        close(pipe_in[WRITEOUT]);
        retval = 0;
    }else{//子进程
        char cmdarg[2048];
        char onearg[2048];
        char *pos = NULL;
        int i=0;
        memset(onearg,0,2048);
        for (i = 0; i < num; i++) {
            sprintf(cmdarg,"%s %s",onearg,arg[i]);
        }
        close(pipe_out[READIN]);
        dup2(pipe_out[WRITEOUT],1);
        close(pipe_out[WRITEOUT]);
        close(pipe_in[WRITEOUT]);
        dup2(pipe_in[READIN],0);
        close(pipe_in[READIN]);

        execlp(rpath,arg);
    }

}

