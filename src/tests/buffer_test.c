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
#include "libutil/buffer.h"
#include "libutil/vector.h"

void test1()
{
    good_buffer_t *a = good_buffer_alloc2(1000);

   good_buffer_printf(a,"12345678");

   good_buffer_t *b = good_buffer_copy(a);

   good_buffer_printf(b,"aaaaaa");

    printf("a={%s}\n",(char*)a->data);
    printf("b={%s}\n",(char*)b->data);

   good_buffer_freep(&a);
   good_buffer_freep(&b);

   void* c = good_heap_alloc(1000);

   good_buffer_t *d = good_heap_alloc(sizeof(good_buffer_t));

    d->data = c;
    d->size = 1000;

    good_buffer_printf(d,"aaaaaa");

    printf("d={%s}\n",(char*)d->data);

    good_buffer_t *d2 =good_buffer_copy(d);

    good_heap_freep((void**)&d);
    good_heap_freep(&c);
    good_buffer_freep(&d2);

    good_buffer_t *e = good_buffer_alloc2(1000*100000);

    good_buffer_t *f = good_buffer_copy(e);
    good_buffer_t *g = good_buffer_copy(f);

    good_buffer_t *h = good_buffer_clone(g);

    good_buffer_freep(&e);
    good_buffer_freep(&f);

    good_buffer_freep(&g);
    good_buffer_freep(&h);
}

void test2()
{
    good_buffer_t *a = good_buffer_alloc2(10);

    printf("%lu\n",good_buffer_write(a,"aaaaaa",6));

    printf("%lu\n",good_buffer_printf(a,"%s","bb"));

    printf("\r%lu\n",good_buffer_fill(a,'c'));

    printf("%lu\n" ,good_buffer_write(a,"dddd",6));

    char buf[11] = {0};

    fprintf(stderr,"%lu\n",good_buffer_read(a,buf,7));

    good_buffer_drain(a);

    fprintf(stderr,"%lu\n",good_buffer_read(a,buf,7));

    fprintf(stderr,"%lu\n",good_buffer_write(a,"abcdefg",6));

    good_buffer_resize(a,1000);

    good_buffer_freep(&a);
}

void test3()
{
    good_buffer_t *a = good_buffer_alloc(NULL);

    good_buffer_t *b = good_buffer_copy(a);

    good_buffer_t *c = good_buffer_clone(b);

    good_buffer_freep(&a);
    good_buffer_freep(&b);
    good_buffer_freep(&c);
}

void test4()
{
    good_vector_t* v = good_vector_alloc2(10,0);
    good_vector_t* v2 = good_vector_copy(v);
    good_vector_t* v3 = good_vector_clone(v);

    good_vector_freep(&v);
    good_vector_freep(&v2);
    good_vector_freep(&v3);

    v = good_vector_alloc2(100,100);
    v2 = good_vector_copy(v);
    v3 = good_vector_clone(v);

    good_vector_freep(&v);
    good_vector_freep(&v2);
    good_vector_freep(&v3);
}

int main(int argc, char **argv)
{
   
   test1();

   test2();

    test3();

    test4();

   return 0;
}