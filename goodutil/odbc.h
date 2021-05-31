/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_ODBC_H
#define GOODUTIL_ODBC_H

#include "general.h"
#include "allocator.h"

#ifdef __SQLEXT_H

/**
 * ODBC接口。
*/
typedef struct _good_odbc
{
    /** 环境。 */
    SQLHENV env;

    /** 连接。 */
    SQLHDBC dbc;

    /** 数据集。 */
    SQLHSTMT stmt;

    /** 
     * 数据集属性。
     * 
     * @note 尽量不要直接修改。
     */
    good_allocator_t *attr;

} good_odbc_t;

/**
 * 释放数据集属性。
*/
void good_odbc_free_attr(good_odbc_t *ctx);

/**
 * 创建数据集属性。
 * 
 * @warning 同一份数据集只会创建一次。
*/
SQLRETURN good_odbc_alloc_attr(good_odbc_t *ctx);

/**
 * 断开连接。
*/
SQLRETURN good_odbc_disconnect(good_odbc_t *ctx);

/**
 * 连接数据库。
 * 
 * @warning 默认：禁用自动提交。
 * 
 * @param timeout 超时(秒)，仅登录时有效。
 * @param tracefile 跟踪文件，NULL(0) 忽略。
*/
SQLRETURN good_odbc_connect(good_odbc_t *ctx, const char *uri,time_t timeout,const char *tracefile);

/**
 * 连接数据库。
 * 
 * @warning 默认：使用TCP连接。
 * 
 * @param product 产品名称的指针。支持：DB2/MYSQL/ORACLE/SQLSERVER/POSTGRESQL
 * @param driver 驱动名称的指针。见：/etc/odbcinst.ini
 * @param host 主机地址的指针。
 * @param port 主机端口。
 * @param db 数据库名称的指针。
 * @param user 登录用名称的指针。
 * @param pwd 登录密码的指针。
 * 
*/
SQLRETURN good_odbc_connect2(good_odbc_t *ctx, const char *product, const char *driver,
                             const char *host, uint16_t port, const char *db,
                             const char *user, const char *pwd,
                             time_t timeout, const char *tracefile);

/**
 * 启用或禁用自动提交。
 * 
 * @param enable !0 启用，0 禁用。
*/
SQLRETURN good_odbc_autocommit(good_odbc_t *ctx,int enable);

/**
 * 开启事务(关闭自动提交)。
*/
#define good_odbc_tran_begin(ctx)   good_odbc_autocommit(ctx,0)

/**
 * 结束事务。
*/
SQLRETURN good_odbc_tran_end(good_odbc_t *ctx, SQLSMALLINT type);

/**
 * 提交事务。
*/
#define good_odbc_tran_commit(ctx) good_odbc_tran_end(ctx, SQL_COMMIT)

/**
 * 回滚事务。
*/
#define good_odbc_tran_rollback(ctx) good_odbc_tran_end(ctx, SQL_ROLLBACK)

/**
 * 准备SQL语句。
 * 
 * @warning 默认：启用静态游标。
*/
SQLRETURN good_odbc_prepare(good_odbc_t *ctx, const char *sql);

/**
 * 执行SQL语句。
 * 
 * @return SQL_SUCCESS(0) 成功，SQL_NO_DATA(100) 无数据，< 0 失败。
*/
SQLRETURN good_odbc_execute(good_odbc_t *ctx);

/**
 * 关闭数据集。
 * 
 * @note 在good_odbc_execute之前执行有意义，其它情况可以不必执行，数据集允许复用。
*/
SQLRETURN good_odbc_finalize(good_odbc_t *ctx);

/**
 * 直接执行SQL语句。
 * 
 * @see good_odbc_prepare
 * @see good_odbc_execute
 * 
*/
SQLRETURN good_odbc_exec_direct(good_odbc_t *ctx,const char *sql);

/**
 * 返回影响的行数。
*/
SQLRETURN good_odbc_affect(good_odbc_t *ctx,SQLLEN *rows);

/**
 * 在数据集中移动游标。
 * 
 * @return SQL_SUCCESS(0) 成功，SQL_NO_DATA(100) 无数据(游标已经在数据集的首或尾)，< 0 失败。
*/
SQLRETURN good_odbc_fetch(good_odbc_t *ctx, SQLSMALLINT direction, SQLLEN offset);

/**
 * 在数据集中移动游标到首行。
*/
#define good_odbc_fetch_first(ctx) good_odbc_fetch(ctx, SQL_FETCH_FIRST, 0)

/**
 * 在数据集中向下移动游标。
*/
#define good_odbc_fetch_next(ctx) good_odbc_fetch(ctx, SQL_FETCH_NEXT, 0)


/** 
 * 获取数据集中指定字段的值。
 * 
 * @param max 缓存区最大长度，值超过这个长度的则会被截断。
 * @param len 字段值长度的指针，返回前填充，NULL(0)忽略。
 * 
*/
SQLRETURN good_odbc_get_data(good_odbc_t *ctx, SQLSMALLINT column, SQLSMALLINT type,
                             SQLPOINTER buf, SQLULEN max, SQLULEN *len);

/**
 * 在数据集中查找字段的索引。
 * 
 * @return >= 0 成功(索引)，< 0 失败(未找到)。
*/
SQLSMALLINT good_odbc_name2index(good_odbc_t *ctx, const char *name);

#endif //__SQLEXT_H

#endif //GOODUTIL_ODBC_H