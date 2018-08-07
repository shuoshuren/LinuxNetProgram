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

enum{
    MINET_HTML,MINET_HTM,MINET_TXT,MINET_CSS,MINET_ICO,MINET_GIF,
    MINET_JPG,MINET_JPEG,MINET_PNG,MINET_SVG,MINET_TORRENT,MINET_WAV,MINET_MP3,
    MINET_MID,MINET_M3U,MINET_RAM,MINET_RA,MINET_DOC,MINET_EXE,MINET_ZIP,MINET_XLS,
    MINET_TGZ,MINET_TARGZ,MINET_TAR,MINET_GZ,MINET_ARJ,MINET_RAR,MINET_RTF,
    MINET_PDF,MINET_SWF,MINET_MPG,MINET_MPEG,MINET_ASF,MINET_AVI,MINET_BMP
};

struct mine_type{
    char *extension;//扩展名
    int type;//类型
    int ext_len;//扩展名长度
    char *mime_type;//内容类型
};

struct mine_type builtin_mine_types[] = {
        {"html",MINET_HTML,4,"text/html"},
        {"htm",MINET_HTM,3,"text/html"},
        {"txt",MINET_TXT,3,"text/plain"},
        {"css",MINET_CSS,3,"text/css"},
        {"ico",MINET_ICO,3,"image/x-icon"},
        {"gif",MINET_GIF,3,"image/gif"},
        {"jpg",MINET_JPG,3,"image/jpeg"},
        {"jpeg",MINET_JPEG,4,"image/jpeg"},
        {"png",MINET_PNG,3,"image/png"},
        {"svg",MINET_SVG,3,"image/svg+xml"},
        {"torrent",MINET_TORRENT,7,"application/x-bittorrent"},
        {"wav",MINET_WAV,3,"audio/x-wav"},
        {"mp3",MINET_MP3,3,"audio/x-mp3"},
        {"mid",MINET_MID,3,"audio/mid"},
        {"m3u",MINET_M3U,3,"audio/x-mpegurl"},
        {"ram",MINET_RAM,3,"audio/x-pn-realaudio"},
        {"ra",MINET_RA,2,"audio/x-pn-realaudio"},
        {"doc",MINET_DOC,3,"application/msword"},
        {"exe",MINET_EXE,3,"application/octet-stream"},
        {"zip",MINET_ZIP,3,"application/x-zip-compressed"},
        {"xls",MINET_XLS,3,"application/excel"},
        {"tgz",MINET_TGZ,3,"application/x-tar-gz"},
        {"tar.gz",MINET_TARGZ,6,"application/x-tar-gz"},
        {"tar",MINET_TAR,3,"application/x-tar"},
        {"gz",MINET_GZ,2,"application/x-gunzip"},
        {"arj",MINET_ARJ,3,"application/x-arj-compressed"},
        {"rar",MINET_RAR,3,"application/x-arj-compressed"},
        {"rtf",MINET_RTF,3,"application/rtf"},
        {"pdf",MINET_PDF,3,"application/pdf"},
        {"swf",MINET_SWF,3,"application/x-shockwave-flash"},
        {"mpg",MINET_MPG,3,"video/mpeg"},
        {"mpeg",MINET_MPEG,4,"video/mpeg"},
        {"asf",MINET_ASF,3,"video/x-ms-asf"},
        {"avi",MINET_AVI,3,"video/x-msvideo"},
        {"bmp",MINET_BMP,3,"image/bmp"},
        {NULL,-1,0,NULL}
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

/**
 * 根据输入的扩展名查找内容类型的匹配项
 * @param uri
 * @param len
 * @param wctl
 * @return
 */
struct mine_type * Mine_Type(char *uri,size_t len,struct worker_ctl *wctl);

#endif //SHTTPD_METHOD_H
