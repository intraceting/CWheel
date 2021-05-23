/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "goodutil/general.h"
#include "goodutil/base64.h"

void test_bloom()
{
    size_t s = 10000000000;
    uint8_t* buf = (uint8_t*)good_heap_alloc(s);

    for(size_t i = 0;i<s*8;i++)
    {
        assert(good_bloom_mark(buf,s,i)==0);
    }

    for(size_t i = 0;i<s;i++)
    {
        assert(buf[i]==255);
    }
    

    for(size_t i = 0;i<s*8;i++)
    {
        assert(good_bloom_filter(buf,s,i)==1);
        assert(good_bloom_unset(buf,s,i)==0);
    }

    for(size_t i = 0;i<s;i++)
    {
        assert(buf[i]==0);
    }



    good_heap_free(buf);
}

void test_base64()
{
    //char src[] = {"a"};
   // char src[] = {"ab"};
    //char src[] = {"abc"};
    //char src[] = {"abcdefg1234567"};
    char src[] = {"abcdefghigklmnopqrstuvwxyz"};
    uint8_t dst[100] = {0};
    char src2[100] = {0};

    ssize_t dlen = good_base64_encode(src,strlen(src),NULL,0);

    ssize_t d2len = good_base64_encode(src,strlen(src),dst,dlen);

    printf("dst(%ld)={%s}\n",d2len,dst);

    ssize_t slen = good_base64_decode(dst,d2len,NULL,0);

    ssize_t s2len = good_base64_decode(dst,d2len,src2,slen);

    printf("src2(%ld)={%s}\n",s2len,src2);

}


int main(int argc, char **argv)
{
    //test_bloom();

    test_base64();

    return 0;
}