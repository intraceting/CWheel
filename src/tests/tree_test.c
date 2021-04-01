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
#include "libutil/tree.h"
#include "libutil/buffer.h"

int dump(size_t deep, const good_tree_t *node, void *opaque)
{
    good_tree_fprintf(stderr,deep,node,"%lu\n",node->alloc->sizes[0]);

    // if(deep>=1)
    //     return 0;
    return 1;
}

void traversal(const good_tree_t *root)
{
    printf("\n-------------------------------------\n");

    good_tree_scan(root,0,dump,NULL);

 //   good_heap_freep((void **)&it.stack);

    printf("\n-------------------------------------\n");
}

int main(int argc, char **argv)
{

    good_tree_t *d = good_tree_alloc();

    d->alloc = good_allocator_alloc2(1);

    good_tree_t *n = good_tree_alloc();

    n->alloc = good_allocator_alloc2(2);

    good_tree_insert2(d, n,1);

    good_tree_t *n2 = n = good_tree_alloc();

    n->alloc = good_allocator_alloc2(3);

    good_tree_insert2(d, n,1);

    n = good_tree_alloc();

    n->alloc = good_allocator_alloc2(4);

    good_tree_insert2(d, n,1);

    traversal(d);

    good_tree_t *m = good_tree_alloc();

    m->alloc = good_allocator_alloc2(5);

    good_tree_insert2(n, m,0);

    traversal(d);

    m = good_tree_alloc();

    m->alloc = good_allocator_alloc2(6);

    good_tree_insert2(n, m,0);

    good_tree_t *m6 = m = good_tree_alloc();

    m->alloc = good_allocator_alloc2(7);

    good_tree_insert2(n, m,0);

    good_tree_t *k = good_tree_alloc();

    k->alloc = good_allocator_alloc2(8);

    good_tree_insert2(m, k,0);

    k = good_tree_alloc();

    k->alloc = good_allocator_alloc2(9);

    good_tree_insert2(m, k,0);

    good_tree_t *u = good_tree_alloc();

    u->alloc = good_allocator_alloc2(10);

    good_tree_insert(m, u, k);

    traversal(d);

    good_tree_unlink(m6);

    traversal(d);

    //good_tree_insert_least(d, m6);
    good_tree_insert(d, m6, n2);

    traversal(d);

    good_tree_unlink(m6);
    good_tree_free(&m6);

    traversal(d);


    good_tree_free(&d);

    return 0;
}