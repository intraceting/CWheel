/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_BASE64_H
#define GOODUTIL_BASE64_H

#include "general.h"

/**
* BASE64 编码
*
* @param dst 密文内存指针，为NULL(0)时仅计算密文长度。
* 
* @return 原文编码后的长度。
*/
ssize_t good_base64_encode(const uint8_t *src,size_t slen,char* dst,size_t dmaxlen);

/**
* BASE64 解码
*
* @param dst 原文内存指针，为NULL(0)时仅计算原文长度。
* 
* @return 密文解码后的长度。
*/
ssize_t good_base64_decode(const char *src,size_t slen,uint8_t* dst,size_t dmaxlen);

#endif //GOODUTIL_BASE64_H