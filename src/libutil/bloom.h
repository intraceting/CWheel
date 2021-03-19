/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_BLOOM_H
#define GOOD_UTIL_BLOOM_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>

#include "general.h"

/** 
 * 布隆-插旗
 * 
 * @param size 池大小(Bytes)
 * 
 * @return 0 成功，1 成功（重复操作），-1 参数误错。
*/
int good_bloom_mark(uint8_t* pool,size_t size,size_t number);

/** 
 * 布隆-拔旗
 * 
 * @param size 池大小(Bytes)
 * 
 * @return 0 成功，1 成功（重复操作），-1 参数误错。
 * 
*/
int good_bloom_unset(uint8_t* pool,size_t size,size_t number);

/**
 * 布隆-过滤
 * 
 * @return 0 不存在，1 已存在，-1 参数误错。
*/
int good_bloom_filter(uint8_t* pool,size_t size,size_t number);


#endif //GOOD_UTIL_BLOOM_H