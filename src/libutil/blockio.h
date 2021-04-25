/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_BLOCKIO_H
#define GOOD_UTIL_BLOCKIO_H

#include "general.h"
#include "buffer.h"

/**
 * 以块为单位读数据。
 * 
 * @return > 0 读取的长度，<= 0 读取失败或已到末尾。
*/
ssize_t good_block_read(int fd, void *data, size_t size,good_buffer_t *buf);

/**
 * 以块为单位写数据。
 * 
 * @return > 0 写入的长度，<= 0 写入失败或空间不足。
*/
ssize_t good_block_write(int fd, const void *data, size_t size,good_buffer_t *buf);

/**
 * 以块为单位写补齐数据。
 * 
 * @param stuffing 填充物。
 * 
 * @return > 0 缓存数据全部写完，= 0 缓存无数据或无缓存，< 0 写入失败或空间不足(剩余数据在缓存中)。
*/
int good_block_write_trailer(int fd, uint8_t stuffing,good_buffer_t *buf);


#endif //GOOD_UTIL_BLOCKIO_H
