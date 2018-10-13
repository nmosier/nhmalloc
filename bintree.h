
#ifndef __BINTREE_H
#define __BINTREE_H

#include <stdio.h>

#define BTREE_EMPTY NULL

/* bitmasks for binary tree nodes */
#define BNODE_FREE 0x1
#define BLEFT_FREE 0x2
#define BRIGHT_FREE 0x4
#define BTREE_USED 0x0
typedef struct bnode_t_ {
   struct bnode_t_ *leftp;
   struct bnode_t_ *rightp;
   struct bnode_t_ *parentp;
   size_t size;
   int free;
} bnode_t;

typedef bnode_t *btree_t;


typedef bnode_t memblock_t;
typedef btree_t memblocks_t;


void btree_print(btree_t tree);

#endif
