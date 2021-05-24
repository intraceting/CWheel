/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "sqlite.h"

int good_sqlite_backup(good_sqlite_backup_param *param)
{
    sqlite3_backup *backup_ctx = NULL;
    int chk;

    assert(param != NULL);

    assert(param->dst != NULL && param->dst_name != NULL);
    assert(param->src != NULL && param->src_name != NULL);
    assert(param->step > 0 && param->sleep > 0);

    backup_ctx = sqlite3_backup_init(param->dst, param->dst_name, param->src, param->src_name);
    if (!backup_ctx)
        GOOD_ERRNO_AND_RETURN1(EINVAL, SQLITE_ERROR);

    do
    {
        chk = sqlite3_backup_step(backup_ctx, param->step);

        if (param->progress_cb)
            param->progress_cb(sqlite3_backup_remaining(backup_ctx), sqlite3_backup_pagecount(backup_ctx), param->opaque);

        if (chk == SQLITE_BUSY || chk == SQLITE_LOCKED)
            sqlite3_sleep(param->sleep);

    } while (chk == SQLITE_OK || chk == SQLITE_BUSY || chk == SQLITE_LOCKED);

    sqlite3_backup_finish(backup_ctx);

    return chk;
}

int  good_sqlite_close(sqlite3 *ctx)
{
    assert(ctx != NULL);

    return sqlite3_close(ctx);
}

int good_sqlite_busy_melt(void *opaque,int count)
{
    /**/
    pthread_yield();

    /*
     * 1: try again.
     * 0: not.
    */

    return 1;
}

sqlite3* good_sqlite_open(const char *name)
{
    sqlite3 *ctx = NULL;
    int chk;

    assert(name != NULL);

    chk = sqlite3_open(name, &ctx);
    if(chk != SQLITE_OK)
        GOOD_ERRNO_AND_RETURN1(EINVAL,NULL);

    /*注册忙碌处理函数。*/
    sqlite3_busy_handler(ctx,good_sqlite_busy_melt,ctx);

    return ctx;
}

int good_sqlite_exec(sqlite3 *ctx,const char *sql)
{
    assert(ctx != NULL && sql != NULL);

    return sqlite3_exec(ctx, sql, NULL, NULL, NULL);
}

int good_sqlite_pagesize(sqlite3 * ctx,int size)
{
    char sql[100] = {0};

    assert(ctx != NULL && size > 0);

    assert(size % 4096 == 0);

    snprintf(sql, 100, "PRAGMA page_size = %d;", size);

    return good_sqlite_exec(ctx,sql);
}

int good_sqlite_journal_mode(sqlite3 *ctx, int mode)
{
    int chk;

    assert(ctx != NULL && mode >= GOOD_SQLITE_JOURNAL_OFF && mode <= GOOD_SQLITE_JOURNAL_WAL);

    switch (mode)
    {
    case GOOD_SQLITE_JOURNAL_OFF:
        chk = good_sqlite_exec(ctx, "PRAGMA journal_mode = OFF;");
        break;
    case GOOD_SQLITE_JOURNAL_DELETE:
        chk = good_sqlite_exec(ctx, "PRAGMA journal_mode = DELETE;");
        break;
    case GOOD_SQLITE_JOURNAL_TRUNCATE:
        chk = good_sqlite_exec(ctx, "PRAGMA journal_mode = TRUNCATE;");
        break;
    case GOOD_SQLITE_JOURNAL_PERSIST:
        chk = good_sqlite_exec(ctx, "PRAGMA journal_mode = PERSIST;");
        break;
    case GOOD_SQLITE_JOURNAL_MEMORY:
        chk = good_sqlite_exec(ctx, "PRAGMA journal_mode = MEMORY;");
        break;
    case GOOD_SQLITE_JOURNAL_WAL:
        chk = good_sqlite_exec(ctx, "PRAGMA journal_mode = WAL;");
        break;
    default:
        chk = SQLITE_PERM;
    }

    return chk;
}

int good_sqlite_name2index(sqlite3_stmt *stmt, const char *name)
{
    int count;
    const char *tmp;
    int idx = -1;

    assert(stmt != NULL && name != NULL);

    count = sqlite3_column_count(stmt);

    for (idx = count - 1; idx >= 0; idx--)
    {
        tmp = sqlite3_column_name(stmt, idx);

        if (good_strcmp(name, tmp, 0) == 0)
            break;
    }

    return idx;
}

int itting_sqlite_next(sqlite3_stmt *stmt)
{
    int chk;

    assert(stmt != NULL);

    chk = sqlite3_step(stmt);
    if (chk == SQLITE_ROW)
        return 1;
    else if (chk == SQLITE_DONE)
        return 0;
    else
        return -1;

    return -1;
}