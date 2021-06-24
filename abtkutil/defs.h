/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_DEFS_H
#define ABTKUTIL_DEFS_H

/**/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE

/**/
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <syslog.h>
#include <pthread.h>
#include <ctype.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <libgen.h>
#include <fnmatch.h>
#include <limits.h>
#include <dirent.h>
#include <poll.h>
#include <iconv.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <tar.h>
#include <termios.h>
#include <dlfcn.h>

#ifdef _OPENMP
#include <omp.h>
#endif //_OPENMP

#include <sys/socket.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/mtio.h>
#include <scsi/scsi.h>
#include <scsi/scsi_ioctl.h>
#include <scsi/sg.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>

#ifdef HAVE_SQLITE
#include <sqlite3.h>
#endif //HAVE_SQLITE

#ifdef HAVE_UNIXODBC
#include <sql.h>
#include <sqlext.h>
#endif //HAVE_UNIXODBC

#ifdef HAVE_OPENSSL 
#include <openssl/opensslconf.h>
#include <openssl/opensslv.h>
#include <openssl/err.h>

#ifndef OPENSSL_NO_RSA
#include <openssl/rsa.h>
#include <openssl/pem.h>
#endif //OPENSSL_NO_RSA

#ifndef OPENSSL_NO_AES
#include <openssl/aes.h>
#endif //OPENSSL_NO_AES

#ifndef OPENSSL_NO_HMAC
#include <openssl/ssl.h>
#endif //OPENSSL_NO_HMAC

#ifndef OPENSSL_NO_HMAC
#include <openssl/hmac.h>
#endif //OPENSSL_NO_HMAC

#endif //HAVE_OPENSSL


/**
 * 主版本号。
 * 
*/
#define ABTK_VERSION_MAJOR 1

/** 
 * 副版本号。
 * 
*/
#define ABTK_VERSION_MINOR 1

/**
 * 转换指针类型。
 * 
 * @note 支持针地址偏移(Bytes)。
 * 
*/
#define ABTK_PTR2PTR(T, P, OF) ((T *)(((char *)(P)) + (OF)))

/**/
#define ABTK_PTR2VPTR(P, OF) ABTK_PTR2PTR(void, P, OF)
#define ABTK_PTR2I8PTR(P, OF) ABTK_PTR2PTR(int8_t, P, OF)
#define ABTK_PTR2U8PTR(P, OF) ABTK_PTR2PTR(uint8_t, P, OF)
#define ABTK_PTR2I16PTR(P, OF) ABTK_PTR2PTR(int16_t, P, OF)
#define ABTK_PTR2U16PTR(P, OF) ABTK_PTR2PTR(uint16_t, P, OF)
#define ABTK_PTR2I32PTR(P, OF) ABTK_PTR2PTR(int32_t, P, OF)
#define ABTK_PTR2U32PTR(P, OF) ABTK_PTR2PTR(uint32_t, P, OF)
#define ABTK_PTR2I64PTR(P, OF) ABTK_PTR2PTR(int64_t, P, OF)
#define ABTK_PTR2U64PTR(P, OF) ABTK_PTR2PTR(uint64_t, P, OF)
#define ABTK_PTR2SIZEPTR(P, OF) ABTK_PTR2PTR(ssize_t, P, OF)
#define ABTK_PTR2USIZEPTR(P, OF) ABTK_PTR2PTR(size_t, P, OF)

/**
 * 转换指针对象。
 * 
 * @note 支持针地址偏移(Bytes)。
 * 
*/
#define ABTK_PTR2OBJ(T, P, OF) (*ABTK_PTR2PTR(T, P, OF))

/**/
#define ABTK_PTR2I8(P, OF) ABTK_PTR2OBJ(int8_t, P, OF)
#define ABTK_PTR2U8(P, OF) ABTK_PTR2OBJ(uint8_t, P, OF)
#define ABTK_PTR2I16(P, OF) ABTK_PTR2OBJ(int16_t, P, OF)
#define ABTK_PTR2U16(P, OF) ABTK_PTR2OBJ(uint16_t, P, OF)
#define ABTK_PTR2I32(P, OF) ABTK_PTR2OBJ(int32_t, P, OF)
#define ABTK_PTR2U32(P, OF) ABTK_PTR2OBJ(uint32_t, P, OF)
#define ABTK_PTR2I64(P, OF) ABTK_PTR2OBJ(int64_t, P, OF)
#define ABTK_PTR2U64(P, OF) ABTK_PTR2OBJ(uint64_t, P, OF)
#define ABTK_PTR2SIZE(P, OF) ABTK_PTR2OBJ(ssize_t, P, OF)
#define ABTK_PTR2USIZE(P, OF) ABTK_PTR2OBJ(size_t, P, OF)

/**
 * 数值比较，返回最大值。
 * 
*/
#define ABTK_MAX(A, B) (((A) > (B)) ? (A) : (B))

/**
 * 数值比较，返回最小值。
 * 
*/
#define ABTK_MIN(A, B) (((A) < (B)) ? (A) : (B))

/**
 * 交换两个数值变量的值。
 * 
*/
#define ABTK_INTEGER_SWAP(A, B) ( \
    {                             \
        (A) ^= (B);               \
        (B) ^= (A);               \
        (A) ^= (B);               \
    })

/**
 * 设置出错码，并返回。
*/
#define ABTK_ERRNO_AND_RETURN0(E) ( \
    {                               \
        errno = (E);                \
        return;                     \
    })

/**
 * 设置出错码，并返回值。
*/
#define ABTK_ERRNO_AND_RETURN1(E, V) ( \
    {                                  \
        errno = (E);                   \
        return (V);                    \
    })

/**
 * 设置出错码，并跳转。
*/
#define ABTK_ERRNO_AND_GOTO1(E, M) ( \
    {                                \
        errno = (E);                 \
        goto M;                      \
    })

/**
 * 计算数组大小。
*/
#define ABTK_ARRAY_SIZE(V) (sizeof((V)) / sizeof((V)[0]))


#endif //ABTKUTIL_DEFS_H