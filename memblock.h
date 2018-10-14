#ifndef __MEMBLOCK_H
#define __MEMBLOCK_H

#include <stdio.h>
#include <stdbool.h>

typedef struct memblock_t_ {
   /* basic fields */
   size_t size;
   int free;

   /* list fields */
   struct memblock_t_ *prevp;

   /* btree fields */
   struct memblock_t_ *leftp;
   struct memblock_t_ *rightp;
   struct memblock_t_ *parentp;
} memblock_t;

typedef struct memblocks_t_ {
   /* list fields */
   memblock_t *front;
   memblock_t *back;

   /* btree fields */
   memblock_t *root;
} memblocks_t;


// memblocks operations
void memblocks_init(memblocks_t *memblocks);

// memblock operations
void memblock_insert(void *begin_addr, void *end_addr, memblocks_t *memblocks);
memblock_t *memblock_find(size_t size, memblocks_t *memblocks);
int memblock_split(memblock_t *block, size_t size, memblocks_t *memblocks);
void memblock_free(void *begin_addr, memblocks_t *memblocks);
void memblock_allocate(void *begin_addr, memblocks_t *memblocks);
void memblocks_print(memblocks_t *memblocks);
bool memblocks_validate(memblocks_t *memblocks);

#endif
