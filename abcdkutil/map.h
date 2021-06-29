/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#ifndef ABCDKUTIL_MAP_H
#define ABCDKUTIL_MAP_H

#include "general.h"
#include "tree.h"

__BEGIN_DECLS

/**
 * MAP。
 * 
 * HASH和DEQUE存储结构。
*/
typedef struct _abcdk_map
{
    /**
     * 表格。
     * 
     * @note 尽量不要直接修改。
    */
    abcdk_tree_t *table;

    /**
     * KEY哈希函数。
    */
    uint64_t (*hash_cb)(const void* key,size_t size,void *opaque);

    /**
     * KEY比较函数。
    */
    int (*compare_cb)(const void *key1, const void *key2, size_t size,void *opaque);

    /** 
     * 构造函数。
    */
    void (*construct_cb)(abcdk_allocator_t *alloc, void *opaque);

    /**
     * 析构函数。
    */
    void (*destructor_cb)(abcdk_allocator_t *alloc, void *opaque);

    /**
     * 回显函数。
     * 
     * @return -1 终止，1 继续。
    */
    int (*dump_cb)(abcdk_allocator_t *alloc, void *opaque);

    /**
    * 环境指针。
    */
    void *opaque;
    
}abcdk_map_t;

/**
 * MAP的字段。
*/
enum _abcdk_map_field
{
    /**
     * Bucket 字段索引。
    */
   ABCDK_MAP_BUCKET = 0,
#define ABCDK_MAP_BUCKET     ABCDK_MAP_BUCKET

    /**
     * Key 字段索引。
    */
   ABCDK_MAP_KEY = 0,
#define ABCDK_MAP_KEY        ABCDK_MAP_KEY

    /**
     * Value 字段索引。
    */
   ABCDK_MAP_VALUE = 1
#define ABCDK_MAP_VALUE      ABCDK_MAP_VALUE
};

/**
 * HASH函数。
*/
uint64_t abcdk_map_hash(const void* data,size_t size,void *opaque);

/**
 * 比较函数。
 * 
 * @return > 0 is data1 > data2，0 is data1 == data2，< 0 is data1 < data2。
*/
int abcdk_map_compare(const void *data1, const void *data2, size_t size,void *opaque);

/**
 * 销毁。
*/
void abcdk_map_destroy(abcdk_map_t* map);

/**
 * 初始化。
 * 
 * @return 0 成功，!0 失败。
*/
int abcdk_map_init(abcdk_map_t* map,size_t size);

/**
 * 查找或创建。
 * 
 * @param ksize Key size。
 * @param vsize Value size。 0 仅查找，>0 不存在则创建。
 * 
 * @return !NULL(0) 成功(复制的指针，不需要主动释放)，NULL(0) 不存在或创建失败。
 * 
*/
abcdk_allocator_t* abcdk_map_find(abcdk_map_t* map,const void* key,size_t ksize,size_t vsize);

/**
 * 删除。
*/
void abcdk_map_remove(abcdk_map_t* map,const void* key,size_t ksize);

/**
 * 扫描节点。
 * 
 * 深度优先遍历节点。
*/
void abcdk_map_scan(abcdk_map_t *map);

__END_DECLS

#endif //ABCDKUTIL_MAP_H