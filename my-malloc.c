//Wriiten by Nick Mosier and Searidang Pa

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "my-malloc.h"
#include "memblock.h"

#include "debug.h"

static intptr_t BREAK_INCREMENT = 0x1000;
static void *PROGRAM_BREAK_ADDR = NULL;
memblocks_t memblocks;

void *malloc(size_t size) {
   memblock_t *memblock_header;
   void *memblock;

   // DEBUG
   //    memblocks_print(&memblocks);
   
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
         LOG("mymalloc: expanding the heap...\n");

         /* failed to find sufficiently sized memblock,
          * so reserve more */
         void *old_break, *new_break;
         old_break = sbrk(BREAK_INCREMENT);
         
         if (old_break == (void *) -1) {
            /* handle sbrk error */
            return NULL;
         }

         new_break = (void *) ((char *) old_break + BREAK_INCREMENT);
         memblock_insert(old_break, new_break, &memblocks);

         PROGRAM_BREAK_ADDR = new_break;
         BREAK_INCREMENT *= 2; // double size for next call to sbrk
      }
   } while (memblock_header == NULL);

   memblock_split(memblock_header, size, &memblocks);
   
   memblock = (void *) (memblock_header + 1);
   memblock_allocate(memblock, &memblocks);

   if (DEBUG) {
      if (!memblocks_validate(&memblocks)) {
         LOG("malloc: program exiting due to internal error.\n");
      }
   }

   return memblock;
}

void free(void *ptr) {   
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
   size_t old_size;

   if (ptr) {
      if (size) {
         /* get old size of memblock */
         old_size = ((memblock_t *) ptr - 1)->size;
         new_ptr = malloc(size);
         memcpy(new_ptr, ptr, old_size);
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
