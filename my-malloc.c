//Wriiten by Nick Mosier and Searidang Pa

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "my-malloc.h"
#include "memblock.h"

// debugging macros
#define LOG(str) write(STDERR_FILENO, str, strlen(str))
char eprintf_buf[1000];
#define eprintf(fmt, ...) sprintf(eprintf_buf, fmt, __VA_ARGS__),   \
   LOG(write)


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
   return memblock;
}

void free(void *ptr) {   
   if (ptr) {
      memblock_free(ptr, &memblocks);
   }

      memblocks_print(&memblocks);
}
