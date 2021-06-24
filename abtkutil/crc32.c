/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include "crc32.h"

static int _abtk_crc32_init(void *opaque)
{
    uint32_t *table = (uint32_t *)opaque;

    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++)
    {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++)
        {
            if (c & 1)
                c = 0xEDB88320L ^ (c >> 1);
            else
                c = c >> 1;
        }

        table[i] = c;
    }

    return 0;
}

uint32_t abtk_crc32_sum(const void *data,size_t size,uint32_t old)
{
    static int init = 0;
    static uint32_t table[256] = {0};
    int chk;

    uint32_t sum = ~old;

    assert(data != NULL && size > 0);

    chk = abtk_once(&init, _abtk_crc32_init, table);
    assert(chk >= 0);

    for (size_t i = 0; i < size; i++)
    {
        sum = table[(sum ^ ABTK_PTR2OBJ(uint8_t, data,i)) & 0xFF] ^ (sum >> 8);
    }

    return ~sum;
}