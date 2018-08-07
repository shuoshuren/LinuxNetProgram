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
#endif //SHTTPD_CGI_H
