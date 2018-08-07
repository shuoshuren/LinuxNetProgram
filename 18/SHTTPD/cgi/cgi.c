//
// Created by xc on 18-8-7.
//

#include <memory.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <dirent.h>
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
        generate_dir_file(wctl);
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


/**
 * 判断当前目录下有一个与默认文件名一致的文件，如果没有，则将当前目录下所有目录列表处理
 * @param wctl
 * @return
 */
int generate_dir_file(struct worker_ctl *wctl){
    struct conn_request *req = &wctl->conn.con_req;
    struct conn_response *res = &wctl->conn.con_res;
    char *command = strstr(req->uri,CGISTR) + strlen(CGISTR);
    char *arg[ARGNUM];
    int num=0;
    char *rpath = wctl->conn.con_req.rpath;
    struct stat *fs = &wctl->conn.con_res.fsate;
    //打开目录
    DIR *dir = opendir(rpath);
    if(dir == NULL){
        //错误
        res->status= 500;
        return -1;
    }
    //建立临时文件保存目录列表
    FILE *tempfile;
    char tempbuff[2048];
    int filesize = 0;
    char *uri = wctl->conn.con_req.uri;
    tempfile = tmpfile();
    //建立标题部分字符串
    sprintf(tempbuff,"%s%s%s",
            "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n<HTML>"
            "<HEAD><TITLE>",
            uri,"</TITLE></HEAD>\n");
    fprintf(tempfile,"%s",tempbuff);
    filesize += strlen(tempbuff);
    sprintf(tempbuff,
            "%s %s %s","<BODY><H1>Index of:",
            uri,
            "</H1> <hr><p><i>Date:</i><i>Size:</i></p><hr>");
    fprintf(tempfile,"%s",tempbuff);
    filesize += strlen(tempbuff);
    //读取目录中的文件列表
    struct dirent *de;
#define PATHLENGTH 2048
    char path[PATHLENGTH];
    char tmpath[PATHLENGTH];
    char linkname[PATHLENGTH];
    strcpy(path,rpath);
    if(rpath[strlen(rpath)]!='/'){
        rpath[strlen(rpath)] = '/';
    }
    while ((de = readdir(dir)) != NULL){//读取文件
        memset(tmpath,0,sizeof(tmpath));
        memset(linkname,0,sizeof(linkname));
        if(strcmp(de->d_name,".")){//不是当前目录
            if(strcmp(de->d_name,"..")){//不是父目录
                strcpy(linkname,de->d_name);//将目录名称作为连接名称
            }else{
                strcpy(linkname,"Parent Directory");
            }
            sprintf(tmpath,"%s%s",path,de->d_name);
            stat(tmpath,&fs);
            if(S_ISDIR(fs->st_mode)){//是目录
                sprintf(tempbuff,"<a href=\"%s/\">%s/</a><br/>\n",
                        de->d_name,tmpath);
            }else{
                char size_str[32];
                off_t  size_int;
                size_int = fs->st_size;
                if(size_int <1024){
                    sprintf(size_str,"%d bytes",(int)size_int);

                }else if(size_int < 1024*1024){
                    sprintf(size_str,"%1.2f Kbytes",(float) size_int/1024);
                }else{
                    sprintf(size_str,"%1.2f Mbytes",(float) size_int/(1024*1024);)
                }
                sprintf(tempbuff,"<a href=\"%s\">%s</a><br/>\n",
                        de->d_name,linkname,size_int);
            }
            fprintf(tempfile,"%s",tempbuff);
            filesize += strlen(tempbuff);
        }
    }
    //生成临时文件信息
    fs->st_ctim.tv_sec = time(NULL);
    fs->st_mtim.tv_sec = time(NULL);
    fs->st_size = filesize;
    fseek(tempfile,(long)0,SEEK_SET);


}

