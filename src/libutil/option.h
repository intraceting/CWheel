/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOOD_UTIL_OPTION_H
#define GOOD_UTIL_OPTION_H

#include "general.h"
#include "map.h"

/**
 * 选项。
 * 
 * 支持一对多键值组合。
*/
typedef struct _good_option
{
    /**
     * 表格。
     * 
     * @warning 尽量不要直接修改。
     * 
     * @see good_map_t
     * 
    */
    good_map_t table;

} good_option_t;

/** 
 * 选项的值。
 *
*/
typedef struct _good_option_value
{
    /**
     * 
    */
    char **texts;

    /**
     * 
    */
    size_t count;

} good_option_value_t;

void good_option_destroy(good_option_t *opt);

int good_option_init(good_option_t *opt);

int good_option_set(good_option_t *opt, const char *key, const char *value);

const good_option_value_t* good_option_get(const good_option_t *opt, const char *key);

#endif //GOOD_UTIL_OPTION_H