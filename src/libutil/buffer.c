/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "buffer.h"

/**
 * 缓存区
*/
typedef struct _good_buffer
{
    /**
     * 魔法数
    */
    uint32_t magic;

    /**
     * 'GOOD' ASCII
    */
#define GOOD_BUFFER_MAGIC 0x474F4F44

    /**
    * 引用计数
    */
    int refcount;

    /**
     * 缓存大小
     * 
     * @note 不包括头结构的大小
    */
    size_t size;

    /**
     * 清理回调函数
     * 
    */
    void (*clean_cb)(void *buf, void *opaque);

    /**
     * 私有指针
    */
    void *opaque;

} good_buffer_t;

size_t good_buffer_size(void *buf)
{
    if (!buf)
        return 0;

    good_buffer_t *buf_p = GOOD_PTR2PTR(good_buffer_t, buf, -sizeof(good_buffer_t));

    assert(buf_p->magic == GOOD_BUFFER_MAGIC);

    return buf_p->size;
}

void *good_buffer_alloc(size_t size, void (*clean_cb)(void *buf, void *opaque), void *opaque)
{
    if (size <= 0)
        return NULL;

    good_buffer_t *buf_p = (good_buffer_t *)calloc(1, sizeof(good_buffer_t) + size);

    if (!buf_p)
        return NULL;

    atomic_init(&buf_p->refcount, 1);

    buf_p->magic = GOOD_BUFFER_MAGIC;
    buf_p->size = size;
    buf_p->clean_cb = clean_cb;
    buf_p->opaque = opaque;

    return GOOD_PTR2PTR(void, buf_p, sizeof(good_buffer_t));
}

void *good_buffer_alloc2(size_t size)
{
    return good_buffer_alloc(size, NULL, NULL);
}

void *good_buffer_refer(void *buf)
{
    if (!buf)
        return NULL;

    good_buffer_t *buf_p = GOOD_PTR2PTR(good_buffer_t, buf, -sizeof(good_buffer_t));

    assert(buf_p->magic == GOOD_BUFFER_MAGIC);

    atomic_fetch_add_explicit(&buf_p->refcount, 1, memory_order_relaxed);

    return buf;
}

void good_buffer_unref(void **buf)
{
    if (!buf || !*buf)
        return;

    good_buffer_t *buf_p = GOOD_PTR2PTR(good_buffer_t, *buf, -sizeof(good_buffer_t));

    assert(buf_p->magic == GOOD_BUFFER_MAGIC);

    if (atomic_fetch_add_explicit(&buf_p->refcount, -1, memory_order_acq_rel) == 1)
    {
        if (buf_p->clean_cb)
            buf_p->clean_cb(*buf, buf_p->opaque);

        buf_p->magic = ~(GOOD_BUFFER_MAGIC);
        buf_p->size = 0;
        buf_p->clean_cb = NULL;
        buf_p->opaque = NULL;

        free(buf_p);
        buf_p = NULL;
    }

    /*Set to NULL(0)*/
    *buf = NULL;
}