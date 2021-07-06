/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#ifndef ABCDKUTIL_URI_H
#define ABCDKUTIL_URI_H

#include "general.h"
#include "allocator.h"

/**
 * URI的字段。
*/
typedef enum _abcdk_uri_field
{
    /** 协议名称*/
    ABCDK_URI_SCHEME = 0,
#define ABCDK_URI_SCHEME    ABCDK_URI_SCHEME

    /** 用户名称*/
    ABCDK_URI_USER = 1,
#define ABCDK_URI_USER  ABCDK_URI_USER

    /** 访问密码*/
    ABCDK_URI_PSWD = 2,
#define ABCDK_URI_PSWD  ABCDK_URI_PSWD

    /** 主机地址(包括端口)*/
    ABCDK_URI_HOST = 3,
#define ABCDK_URI_HOST  ABCDK_URI_HOST

    /** 资源路径*/
    ABCDK_URI_PATH = 4,
#define ABCDK_URI_PATH  ABCDK_URI_PATH

    /** 查询条件*/
    ABCDK_URI_COND = 5,
#define ABCDK_URI_COND  ABCDK_URI_COND

    /** 片段ID*/
    ABCDK_URI_SEGM = 6
#define ABCDK_URI_SEGM  ABCDK_URI_SEGM

}abcdk_uri_field;

/**
 * URI拆分。
 * 
*/
abcdk_allocator_t *abcdk_uri_split(const char *uri);

#endif //ABCDKUTIL_URI_H