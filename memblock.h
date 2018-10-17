#ifndef __MEMBLOCK_H
#define __MEMBLOCK_H

   #include "list.h"

// memblocks operations
void memblocks_init(memblocks_t *memblocks);

// memblock operations
void memblock_insert(void *begin_addr, void *end_addr, memblocks_t *memblocks);
memblock_t *memblock_find(size_t size, memblocks_t *memblocks);
int memblock_split(memblock_t *block, size_t size, memblocks_t *memblocks);
void memblock_free(void *begin_addr);
void memblock_allocate(void *begin_addr);
int memblocks_validate(memblocks_t *memblocks);

#endif
