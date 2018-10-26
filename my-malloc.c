//Wriiten by Nick Mosier

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "memblock.h"
#include "btree.h"
#include "list.h"
#include "debug.h"

//static intptr_t BREAK_INCREMENT = 0x1000;
//static void *PROGRAM_BREAK_ADDR = NULL;
static memblocks_t memblocks = {0};

void *malloc(size_t size) {
   memblock_t *memblock_header;
   void *memblock;

   /* check size is nonzero */
   if (size == 0) {
      return NULL;
   }

   /* initialize globals if necessary */
   if (memblocks.break_addr == NULL) {
      /* intialize mymalloc globals */
      memblocks.break_addr = sbrk(0);
      memblocks_init(&memblocks);
   }

   /* try to find suitable segment in list of memory blocks */
   do {
      memblock_header = memblock_find(size, &memblocks);
      
      if (memblock_header == NULL) {
         if (DEBUG) {
            LOG("mymalloc: expanding the heap...\n");
         }

         /* failed to find sufficiently sized memblock,
          * so reserve more */
         void *old_break, *new_break;
         old_break = sbrk(memblocks.break_inc);


         if (old_break == (void *) -1) {
            /* handle sbrk error */
            return NULL;
         }

         if (DEBUG) {
            assert (old_break == memblocks.break_addr);
         }
         
         new_break = (void *) ((char *) old_break + memblocks.break_inc);
         memblock_insert(old_break, new_break, &memblocks);

         memblocks.break_addr = new_break;
         memblocks.break_inc *= 2; // double size for next call to sbrk
      }
   } while (memblock_header == NULL);

   if (DEBUG) {
      assert(memblock_header->free);
   }
   
   memblock_split(memblock_header, size, &memblocks);
   
   memblock = (void *) (memblock_header + 1);
   memblock_allocate(memblock, &memblocks);

   if (DEBUG) {
      if (!memblocks_validate(&memblocks)) {
         LOG("malloc: program exiting due to internal error.\n");
         exit(1);
      }
      memset(memblock, 0, size); // make sure its writable
   }

   return memblock;
}

static int btree_maxheight_left = 0, btree_maxheight_right = 0, btree_maxcount = 0;
void free(void *ptr) {
   if (DEBUG && ptr) {
      memblock_t *header = (memblock_t *) ptr - 1;
      size_t size = header->size;
      memset(ptr, 0, size);
   }
   
   if (ptr) {
      memblock_free(ptr, &memblocks);
   }

   if (DEBUG) {
      if (!memblocks_validate(&memblocks)) {
         LOG("free: program exiting due to internal malloc error.\n");
         exit(1);
      }
   }

   if (DEBUG && memblocks.root) {
      int height;
      
      height = btree_height(memblocks.root->leftp);
      if (height > btree_maxheight_left) {
         btree_maxheight_left = height;
      }
      height = btree_height(memblocks.root->rightp);
      if (height > btree_maxheight_right) {
         btree_maxheight_right = height;
      }

      int count = btree_count(memblocks.root);
      if (count > btree_maxcount) { btree_maxcount = count; }
   }
}

void *calloc(size_t nmemb, size_t size) {
   void *ptr = NULL;
   size_t bytes = nmemb * size;

   if (bytes) {
      /* mallocate memory */
      ptr = malloc(bytes);
      /* zero out memory */
      memset(ptr, 0, bytes);
   }

   return ptr;
}

void *realloc(void *ptr, size_t size) {
   void *new_ptr;
   memblock_t *old_memblock, *prev_memblock, *next_memblock, *merged_memblock, *free_memblock;
   size_t old_size, cpy_size;
   
   if (ptr) {
      if (size) {
         /* get previous, old, and next memblocks */
         old_memblock = (memblock_t *) ptr - 1;
         old_size = old_memblock->size;
         prev_memblock = old_memblock->prevp;
         next_memblock = (ptr < (void *) (memblocks.back + 1)) ? (memblock_t *) ((char *) ptr + old_size) : NULL;
         cpy_size = (old_size < size) ? old_size : size;



         /* check for any adjacent free blocks:
          * 1. check next_memblock first to avoid a copy
          * 2. check prev_memblock second to avoid fragmentation
          * 3. otherwise, alloc new block
          */
         free_memblock = NULL;
         merged_memblock = old_memblock;
         if (merged_memblock->size < size && next_memblock) {
            merged_memblock = memblock_merge(merged_memblock, MEMBLOCK_MERGE_NEXT, &memblocks);

            if (DEBUG) {
               memblocks_validate(&memblocks);
            }
         }
         if (merged_memblock->size < size && prev_memblock) {
            merged_memblock = memblock_merge(merged_memblock, MEMBLOCK_MERGE_PREV, &memblocks);

            if (DEBUG) {
               memblocks_validate(&memblocks);
            }
         }
         if (merged_memblock->size < size) {
            /* allocate new pointer */
            free_memblock = merged_memblock; // save old memblock to free later
            merged_memblock = ((memblock_t *) malloc(size)) - 1;
         }

         new_ptr = (void *) (merged_memblock + 1);
         if (merged_memblock != old_memblock) {
            /* copy old contents to new memblock */
            memcpy(new_ptr, ptr, cpy_size);
         }

         if (free_memblock) {
            /* free merged_memblock */
            free((void *) (free_memblock + 1));
         }

         /* split off any extra space */
         memblock_split(merged_memblock, size, &memblocks);
         
         if (DEBUG) {
            memblocks_validate(&memblocks);
         }
         
         return new_ptr;
      } else {
         /* size = 0, same as free */
         free(ptr);
         return NULL;
      }
   } else {
      /* same as malloc */
      return malloc(size);
   }
}
