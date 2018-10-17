#include <stdbool.h>

#include "memblock.h"
#include "btree.h"
#include "list.h"
#include "assert.h"
#include "debug.h"

/* memblocks_init()
 * DESC: initialize empty memblocks structure
 * PARAMS:
 *  - memblocks: pointer to memblocks struct to intialiaze in place
 */
void memblocks_init(memblocks_t *memblocks) {
   memblocks->root = memblocks->front = memblocks->back = NULL;
   return;
}

/* memblocks_find()
 * DESC: find free memblock of minimum size _size_
 * PARAMS:
 *  - size: minimum size of memblock
 *  - memblocks: ptr to memblocks structure
 * RETVAL: returns ptr to free memblock if one found;
 *         returns NULL if none found
 */
memblock_t *memblock_find(size_t size, memblocks_t *memblocks) {
   /* find minimum sized node greater than or equal to _size_ */
   return btree_minlwrbnd(size, memblocks->root);
}

/* memblocks_insert()
 * DESC: construct and insert new free memory block into memblocks structure
 * PARAMS:
 *  - begin_addr: beginning of memory block
 *  - end_addr: end of memory block
 *  - memblocks: ptr to memblocks structure
 * NOTE: insertion only occurs when a new block is being added to the 
 *       memblocks structure, so it is always added at the end of the lsit
 */
void memblock_insert(void *begin_addr, void *end_addr, memblocks_t *memblocks) {
   memblock_t *header;

   /* initialize memblock at begin_addr */
   header = (memblock_t *) begin_addr;
   header->size = (char *) end_addr - (char *) begin_addr - sizeof(memblock_t);
   header->free = true;
   
   /* insert into free tree & list */
   memblocks->root = btree_insert(header, memblocks->root);
   list_append(header, memblocks);   
}

/* memblock_split()
 * DESC: splits memory block into two different blocks, the first of size
 *       _size_ and the second of the remaining size
 * PARAMS:
 *  - block: block to be split
 *  - size: allocatable size of first block after being split
 *  - memblocks: ptr to memblocks structure
 * NOTES:
 *  - will only split the memblock if both resulting blocks will be
 *    nonzero in size
 *  - assumes that block is free (why else would it be split?)
 */
int memblock_split(memblock_t *block, size_t size, memblocks_t *memblocks) {
   memblock_t *block2, *root;
   size_t total_size = block->size;
   size_t remaining_size;

   /* check if there's enough space for 2nd block */
   if (total_size <= size + sizeof(memblock_t)) {
      return false;
   }

   /* split blocks */
   remaining_size = total_size - size - sizeof(memblock_t);
   block->size = size; // update size of first block
   block2 = (bnode_t *) ((char *) (block + 1) + size);

   /* initialize new block */
   block2->size = remaining_size;
   block2->free = true; // assume free

   /* remove original block from tree */
   memblocks->root = btree_remove(block, memblocks->root);
   
   /* insert new blocks/block into tree/list */
   root = btree_insert(block, memblocks->root);
   memblocks->root = btree_insert(block2, root);
   list_insert(block2, memblocks);

   return true;
}

/* memblock_free()
 * DESC: frees block of memory (called by free())
 * PARAMS:
 *  - begin_addr: the beginning address of the ALLOCATED
 *                portion of the block
 *  - memblocks: memblocks structure ptr
 */
void memblock_free(void *begin_addr, memblocks_t *memblocks) {
   memblock_t *block = ((memblock_t *) begin_addr) - 1;

   if (DEBUG) {
      assert (block->free == false);
   }
   
   block->free = true;
   memblocks->root = btree_insert(block, memblocks->root);
}

void memblock_allocate(void *begin_addr, memblocks_t *memblocks) {
   memblock_t *block = ((memblock_t *) begin_addr) - 1;

   if (DEBUG) {
      assert (block->free == true);
   }

   memblocks->root = btree_remove(block, memblocks->root);

   /* note: make block false after removal as to avoid
    *       violating the invariant that the btree only
    *       contains free memblocks
    */
   block->free = false;
}



void memblocks_print(memblocks_t *memblocks) {
   list_print(memblocks);
   btree_print(memblocks->root);
}


bool memblocks_validate(memblocks_t *memblocks) {
   const memblock_t *invalid;

   if ((invalid = btree_validate(memblocks->root))) {
      char sbuf[1000];
      sprintf(sbuf, "memblocks_validate: "COLOR_RED"invalid btree node"  \
              COLOR_RESET" @ %p", (void *) invalid);
      btree_perror(sbuf);
      LOG("dumping tree...\n");
      btree_print(memblocks->root);
      return false;
   }
   if ((invalid = list_validate(memblocks))) {
      char sbuf[1000];
      sprintf(sbuf, "memblocks_validate: "COLOR_RED"invalid list node"   \
              COLOR_RESET" @ %p", (void *) invalid);
      list_perror(sbuf);
      LOG("dumping list...\n");
      list_print(memblocks);
      return false;
   }
   
   return true;
}
