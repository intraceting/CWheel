/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#ifndef ABTKUTIL_SQLITE_H
#define ABTKUTIL_SQLITE_H

#include "general.h"

#ifdef _SQLITE3_H_

/**
 * 字段类型。
 * 
*/
enum _abtk_sqlite_column_type
{
    ABTK_SQLITE_COLUMN_INT = 1,
#define ABTK_SQLITE_COLUMN_INT ABTK_SQLITE_COLUMN_INT

    ABTK_SQLITE_COLUMN_INT64 = 2,
#define ABTK_SQLITE_COLUMN_INT64 ABTK_SQLITE_COLUMN_INT64

    ABTK_SQLITE_COLUMN_DOUBLE = 3,
#define ABTK_SQLITE_COLUMN_DOUBLE ABTK_SQLITE_COLUMN_DOUBLE

    ABTK_SQLITE_COLUMN_VARCHAR = 4,
#define ABTK_SQLITE_COLUMN_VARCHAR ABTK_SQLITE_COLUMN_VARCHAR

    ABTK_SQLITE_COLUMN_BLOB = 5
#define ABTK_SQLITE_COLUMN_BLOB ABTK_SQLITE_COLUMN_BLOB

};

/**
 * 日志模式。
*/
enum _abtk_sqlite_journal_mode
{
    ABTK_SQLITE_JOURNAL_OFF = 0,
#define ABTK_SQLITE_JOURNAL_OFF ABTK_SQLITE_JOURNAL_OFF

    ABTK_SQLITE_JOURNAL_DELETE = 1,
#define ABTK_SQLITE_JOURNAL_DELETE ABTK_SQLITE_JOURNAL_DELETE

    ABTK_SQLITE_JOURNAL_TRUNCATE = 2,
#define ABTK_SQLITE_JOURNAL_TRUNCATE ABTK_SQLITE_JOURNAL_TRUNCATE

    ABTK_SQLITE_JOURNAL_PERSIST = 3,
#define ABTK_SQLITE_JOURNAL_PERSIST ABTK_SQLITE_JOURNAL_PERSIST

    ABTK_SQLITE_JOURNAL_MEMORY = 4,
#define ABTK_SQLITE_JOURNAL_MEMORY ABTK_SQLITE_JOURNAL_MEMORY

    ABTK_SQLITE_JOURNAL_WAL = 5
#define ABTK_SQLITE_JOURNAL_WAL ABTK_SQLITE_JOURNAL_WAL
};

/**
 * 备份参数
*/
typedef struct _abtk_sqlite_backup_param
{
    /**目标库的指针。*/
    sqlite3 *dst;

    /**目标库的名字的指针。*/
    const char *dst_name;

    /**源库的指针。*/
    sqlite3 *src;

    /**源库的名字的指针。*/
    const char *src_name;

    /**备份步长(页数量)。*/
    int step;

    /*忙时休息时长(毫秒)。*/
    int sleep;

    /**
     * 进度函数。
     * 
     * @param remaining  剩余页数量。
     * @param total 总页数量。
     * @param opaque 环境指针。
    */
    void (*progress_cb)(int remaining, int total, void *opaque);

    /**环境指针。*/
    void *opaque;

} abtk_sqlite_backup_param;

/**
 * 备份。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
 * 
 */
int abtk_sqlite_backup(abtk_sqlite_backup_param *param);

/**
 * 关闭数据库句柄。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
 * 
*/
int abtk_sqlite_close(sqlite3 *ctx);

/**
 * 忙碌处理函数。
*/
int abtk_sqlite_busy_melt(void *opaque, int count);

/**
 * 打开数据库文件。
 * 
 * @param name 数据库文件名的指针。
 * 
 * @return !NULL(0) 成功(句柄)，NULL(0) 失败。
 * 
*/
sqlite3 *abtk_sqlite_open(const char *name);

/**
 * 打开内存数据库。
*/
#define abtk_sqlite_memopen() abtk_sqlite_open(":memory:")

/**
 * 启动事物。
*/
#define abtk_sqlite_tran_begin(ctx) sqlite3_exec(ctx, "begin;", NULL, NULL, NULL)

/**
 * 提交事物。
*/
#define abtk_sqlite_tran_commit(ctx) sqlite3_exec(ctx, "commit;", NULL, NULL, NULL)

/**
 * 回滚事物。
*/
#define abtk_sqlite_tran_rollback(ctx) sqlite3_exec(ctx, "rollback;", NULL, NULL, NULL)

/**
 * 回收空间。
*/
#define abtk_sqlite_tran_vacuum(ctx) sqlite3_exec(ctx, "vacuum;", NULL, NULL, NULL)

/** 
 * 设置页大小。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
 * 
*/
int abtk_sqlite_pagesize(sqlite3 *ctx, int size);

/** 
 * 设置日志模式。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
 * 
*/
int abtk_sqlite_journal_mode(sqlite3 *ctx, int mode);

/**
 * 准备SQL语句。
 * 
 * @return !NULL(0) 成功(数据集指针)，NULL(0) 失败。
 * 
*/
sqlite3_stmt* abtk_sqlite_prepare(sqlite3 *ctx,const char *sql);

/** 
 * 提交语句，或在数据集中移动游标到下一行。
 * 
 * @return > 0 有数据返回，= 0 无数返回(或末尾)。< 0 出错。
 * 
*/
int abtk_sqlite_step(sqlite3_stmt *stmt);

/**
 * 关闭数据集。
 * 
 * @return SQLITE_OK(0) 成功，!SQLITE_OK(0) 失败。
 * 
*/
int abtk_sqlite_finalize(sqlite3_stmt *stmt);

/**
 * 直接执行SQL语句。
 * 
 * @warning 不能用于返回数据集。
 * 
 * @return >= 0 成功。< 0 出错。
 * 
*/
int abtk_sqlite_exec_direct(sqlite3 *ctx,const char *sql);

/**
 * 在数据集中查找字段的索引。
 * 
 * @return >= 0 成功(索引)，< 0 失败(未找到)。
 * 
*/
int abtk_sqlite_name2index(sqlite3_stmt *stmt, const char *name);


#endif //_SQLITE3_H_

#endif //ABTKUTIL_SQLITE_H