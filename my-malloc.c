//Wriiten by Nick Mosier and Searidang Pa

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "memblock.h"
#include "debug.h"
#include "signal.h"

static intptr_t BREAK_INCREMENT = 0x1000;
static void *PROGRAM_BREAK_ADDR = NULL;
memblocks_t memblocks;

void *malloc(size_t size) {
   /* TEMPORARY: try 8-byte alignment */
   //         eprintf("%zu\n", size);

   memblock_t *memblock_header;
   void *memblock;

   /* check size is nonzero */
   if (size == 0) {
      return NULL;
   }

   /* initialize globals if necessary */
   if (PROGRAM_BREAK_ADDR == NULL) {
      /* intialize mymalloc globals */
      PROGRAM_BREAK_ADDR = sbrk(0);
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
         old_break = sbrk(BREAK_INCREMENT);


         if (old_break == (void *) -1) {
            /* handle sbrk error */
            return NULL;
         }

         //         if (old_break != PROGRAM_BREAK_ADDR) {
            /* another program sneakily moved the program break,
             * so insert this useless chunk into the list */
         // memblock_insert(PROGRAM_BREAK_ADDR, old_break, &memblocks);
         // memblock_allocate((memblock_t *) PROGRAM_BREAK_ADDR + 1, &memblocks);
         //         }
         
         if (DEBUG) {
            assert (old_break == PROGRAM_BREAK_ADDR);
         }
         
         new_break = (void *) ((char *) old_break + BREAK_INCREMENT);
         memblock_insert(old_break, new_break, &memblocks);

         PROGRAM_BREAK_ADDR = new_break;
         BREAK_INCREMENT *= 2; // double size for next call to sbrk
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

   //   eprintf("%p\n", (void *) memblock);
   return memblock;
}

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
   // IMPROVEMENT: look to see if there's contiguous memory after this
   void *new_ptr;
   memblock_t *old_memblock;
   size_t old_size, cpy_size;

   if (ptr) {
      if (size) {
         old_memblock = (memblock_t *) ptr - 1;
         old_size = old_memblock->size;

         /*
         new_ptr = memblock_merge(old_memblock, MEMBLOCK_MERGE_NEXTP, &memblocks);
         if (new_ptr == NULL) {
            new_ptr = memblock_merge(old_memblock, MEMBLOCK_MERGE_PREVP, &memblocks);
            }
         if (new_ptr == NULL) {
            new_ptr = malloc(size);
            } */
         new_ptr = malloc(size);
         cpy_size = (old_size < size) ? old_size : size;
         memcpy(new_ptr, ptr, cpy_size);
         free(ptr);
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
