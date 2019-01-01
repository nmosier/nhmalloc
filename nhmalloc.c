//Wriiten by Nick Mosier

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "nhmalloc.h"
#include "memblock.h"
#include "btree.h"
#include "list.h"
#include "debug.h"

static memblocks_t memblocks = {0};

void *malloc(size_t size) {
   memblock_t *memblock_header;
   void *new_ptr;

   /* check size is nonzero */
   if (size == 0) {
      return NULL;
   }

   /* initialize globals if necessary */
   if (memblocks.break_addr == NULL) {
      /* intialize nhmalloc globals */
      memblocks.break_addr = sbrk(0);
      memblocks_init(&memblocks);
   }

   /* try to find suitable segment in list of memory blocks */
   do {
      memblock_header = memblock_find(size, &memblocks);
      
      if (memblock_header == NULL) {
         if (DEBUG) {
            LOG("nhmalloc: expanding the heap...\n");
         }

         /* failed to find sufficiently sized memblock,
          * so reserve more */
         void *old_break, *new_break;//, *old_break_aligned, *new_break_aligned;
         old_break = sbrk(memblocks.break_inc);


         if (old_break == (void *) -1) {
            /* handle sbrk error */
            return NULL;
         }

         if (DEBUG) {
            assert (old_break == memblocks.break_addr);
         }
         
         new_break = (void *) ((char *) old_break + memblocks.break_inc);

         memblocks.break_addr = new_break;
         memblocks.break_inc *= 2; // double size for next call to sbrk

         memblock_insert(old_break, new_break, &memblocks);
      }
   } while (memblock_header == NULL);

   if (DEBUG) {
      assert(memblock_header->free);
   }
   
   memblock_split(memblock_header, size, &memblocks);
   
   new_ptr = memblock2ptr(memblock_header);
   memblock_allocate(new_ptr, &memblocks);

   if (DEBUG) {
      memblocks_validate(&memblocks);
         //         LOG("malloc: program exiting due to internal error.\n");
         //exit(1);
         //      }
      memset(new_ptr, 0, size); // make sure its writable

      /* make sure properly aligned */
      if (!MALLOC_ALIGN_VALIDATE(new_ptr)) {
         eprintf("nhmalloc: internal error: pointer not %d-byte aligned: %p\n",
                 MALLOC_ALIGN, (void *) new_ptr);
      }
   }

   return new_ptr;
}

static int btree_maxheight_left = 0, btree_maxheight_right = 0, btree_maxcount = 0;
void free(void *ptr) {
   if (DEBUG && ptr) {
      memblock_t *header = ptr2memblock(ptr);
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
         old_memblock = ptr2memblock(ptr);
         old_size = old_memblock->size;
         prev_memblock = memblock_prev(old_memblock, &memblocks);
         next_memblock = memblock_next(old_memblock, &memblocks);
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
            new_ptr = malloc(size);
            if (new_ptr == NULL) {
               /* handle malloc error */
               return NULL;
            }
            
            merged_memblock = ptr2memblock(new_ptr);
         }

         new_ptr = memblock2ptr(merged_memblock);
         if (merged_memblock != old_memblock) {
            /* copy old contents to new memblock */
            memcpy(new_ptr, ptr, cpy_size);
         }

         if (free_memblock) {
            /* free merged_memblock */
            free(memblock2ptr(free_memblock));
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
