/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_SIGNAL_H
#define GOOD_UTIL_SIGNAL_H

#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/file.h>

#include "general.h"
#include "thread.h"

/**
 * 等待信号。
 * 
 * @param timeout >= 0 当信号到达或时间(毫秒)过期即返回，< 0 直到信号到达或出错返回。
 * @param signal_cb 信号处理函数。NULL(0) 忽略信号。返回 !0 继续，0 终止。
 * 
 * @return >=0 成功，< 0 失败或超时。
 * 
*/
int good_sigwaitinfo(sigset_t *sig, time_t timeout,
                     int (*signal_cb)(const siginfo_t *info, void *opaque),
                     void *opaque);

#endif //GOOD_UTIL_SIGNAL_H