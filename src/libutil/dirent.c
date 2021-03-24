/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "dirent.h"

char *good_dirdir(char *path, const char *suffix)
{
    size_t len = 0;

    assert(path != NULL && suffix != NULL);

    len = strlen(path);
    if (len > 0)
    {
        if ((path[len - 1] == '/') && (suffix[0] == '/'))
        {
            path[len - 1] = '\0';
            len -= 1;
        }
        else if ((path[len - 1] != '/') && (suffix[0] != '/'))
        {
            path[len] = '/';
            len += 1;
        }
    }

    /*
     * 要有足够的可用空间，不然会溢出。
    */
    strcat(path + len, suffix);

    return path;
}

void good_mkdir(const char *path, mode_t mode)
{
    size_t len = 0;
    char *tmp = NULL;
    int chk = 0;

    assert(path != NULL);

    len = strlen(path);
    if (len <= 0)
        GOOD_ERRNO_AND_RETURN0(EINVAL);

    tmp = (char *)good_heap_dup(path, len + 1);
    if (!tmp)
        GOOD_ERRNO_AND_RETURN0(ENOMEM);

    /*
     * 必须允许当前用户具有读、写、访问权限。
    */
    mode |= S_IRWXU;

    for (size_t i = 1; i < len; i++)
    {
        if (tmp[i] != '/')
            continue;

        tmp[i] = '\0';

        if (access(tmp, F_OK) != 0)
            chk = mkdir(tmp, mode & (S_IRWXU | S_IRWXG | S_IRWXO));

        tmp[i] = '/';

        if (chk != 0)
            break;
    }

    if (tmp)
        good_heap_freep((void**)&tmp);
}

char *good_dirname(char *dst, const char *src)
{
    char *find = NULL;
    char *path = NULL;

    assert(dst != NULL && src != NULL);

    path = (char *)good_heap_dup(src, strlen(src) + 1);
    if (!path)
        GOOD_ERRNO_AND_RETURN1(ENOMEM, NULL);

    find = dirname(path);
    if (find)
        memcpy(dst, find, strlen(find) + 1);

    good_heap_freep((void**)&path);

    return dst;
}

char *good_basename(char *dst, const char *src)
{
    char *find = NULL;
    char *path = NULL;

    assert(dst != NULL && src != NULL);

    path = (char *)good_heap_dup(src, strlen(src) + 1);
    if (!path)
        GOOD_ERRNO_AND_RETURN1(ENOMEM, NULL);

    find = basename(path);
    if (find)
        memcpy(dst, find, strlen(find) + 1);

    good_heap_freep((void**)&path);

    return dst;
}

char *good_dirnice(char *dst, const char *src)
{
    char *s = NULL;
    char *t = NULL;
    char *d = NULL;
    size_t deep = 0;
    good_buffer_t* stack;
    char* saveptr = NULL;
    
    assert(dst != NULL && src != NULL);

    stack = good_buffer_alloc2(NULL,2048);
    if(!stack)
        goto final;

    d = dst;
    s = good_heap_dup(src,strlen(src)+1);

    if (s == NULL || *s == '\0')
        goto final;

    /*
     * 拆分目录，根据目录层级关系压入堆栈。
    */
    while (1)
    {
        t = good_strtok(s,"/",&saveptr);
        if(!t)
            break;
        
        if(*t == '\0')
            continue;

        if(good_strcmp(t,".",1)==0)
            continue;
        
        if(good_strcmp(t,"..",1)==0)
        {
            if (deep > 0)
                stack->data[--deep] = NULL;
        }    
        else
        {
            assert(deep < stack->number);

            stack->data[deep++] = t;
        }
    }

    /*
     * 拼接目录
    */
    if (*src == '/')
        good_dirdir(dst, "/");

    for (size_t i = 0; i < deep; i++)
    {
        if (i > 0)
             good_dirdir(dst, "/");

        good_dirdir(dst,stack->data[i]);
    }

final:

    good_buffer_unref(&stack);
    good_heap_freep((void**)&s);

    return dst;
}