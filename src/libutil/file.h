/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_FILE_H
#define GOOD_UTIL_FILE_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "general.h"
#include "buffer.h"

/**
 * 写数据。
 * 
 * 当需要定长写入时，或数据生产碎片化比较严重需要优化写入次数时，请启用缓存区。
 * 
 * @param buf 缓存。
 * 
 * @return > 0 写入完成的大小，<= 0 写入失败或空间不足。
 * 
 * @see good_buffer_alloc3()
*/
ssize_t good_write(int fd, const void *data, size_t size, good_buffer_t *buf);

/**
 * 写数据尾。
 * 
 * @param fill !0 填满缓存，0 不填充。
 * @param stuffing 填充字符。
 * 
 * @return > 0 未写完的数据长度，= 0 缓存为空或已经全部写入文件。
 * 
 * @see good_write()
 * 
*/
ssize_t good_write_trailer(int fd, int fill, uint8_t stuffing, good_buffer_t *buf);

/**
 * 读数据。
 * 
 * 当需要定长读取时，或数据消费碎片化比较严重需要优化读取次数时，请启用缓存区。
 * 
 * @param buf 缓存。
 * 
 * @return > 0 读取完成的大小，<= 0 读取失败或文件已经到末尾。
 * 
 * @see good_buffer_alloc3()
 * 
*/
ssize_t good_read(int fd, void *data, size_t size, good_buffer_t *buf);




#endif //GOOD_UTIL_FILE_H