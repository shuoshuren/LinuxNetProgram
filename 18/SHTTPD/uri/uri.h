//
// Created by xc on 18-8-6.
//

#ifndef SHTTPD_URI_H
#define SHTTPD_URI_H

/**
 * 进行字符串转换
 * @param src
 * @param src_len
 * @param dest
 * @param dest_len
 * @return
 */
static int uri_decode(char *src,int src_len,char *dest,int dest_len);

/**
 * 对目录中的“..”进行转换
 * @param s
 */
static void remove_double_dots(char *s);
#endif //SHTTPD_URI_H
