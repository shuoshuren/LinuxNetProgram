//
// Created by xc on 18-8-7.
//

#ifndef SHTTPD_CGI_H
#define SHTTPD_CGI_H

#include "../base/base.h"


#define CGISTR "/cgi-bin"
#define ARGNUM 16
#define READIN 0
#define WRITEOUT 1

int cgi_handler(struct worker_ctl *wctl);

/**
 * 判断当前目录下有一个与默认文件名一致的文件，如果没有，则将当前目录下所有目录列表处理
 * @param wctl
 * @return
 */
int generate_dir_file(struct worker_ctl *wctl);
#endif //SHTTPD_CGI_H
