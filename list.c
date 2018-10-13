// singly linked list implementation used for mymalloc
#include <stdio.h>
#include <stdbool.h>

#include "list.h"
#include "memblock.h"

#define COLOR_RED   "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

/* list_append()
 * 
 * NOTE: list_node NEED NOT BE INITIALIZED, but space must be reserved
 */
void list_append(list_node_t *nodep, list_t *listp) {
   /* if list is empty, node is also front */
   if (listp->front == NULL) {
      listp->front = listp->back = nodep;
   } else {
      listp->back->tail = nodep;
      listp->back = nodep;
   }
   nodep->tail = NULL;
}

void list_prepend(list_node_t *nodep, list_t *listp) {
   nodep->tail = listp->front;
   listp->front = nodep;
}

inline void list_insert(list_node_t *nodep, list_t *listp) {
   list_prepend(nodep, listp);
}

/* list_minlwrbnd() -- find minimum element of list with lower bound
 * RETVAL: ptr to node if found; NULL if no such element found
 */
list_node_t *list_minlwrbnd(size_t lwrbnd, list_t *listp) {
   size_t minval;
   list_node_t *minnode, *list_it;

   minval = (size_t) -1; // maximum value of size_t
   minnode = NULL;
   
   /* iterate thru list */
   for (list_it = listp->front; list_it; list_it = list_it->tail) {
      /* update min val if between lower bound and current min val */
      if (list_it->free && list_it->size >= lwrbnd && list_it->size <= minval) {
         minnode = list_it;
         minval = list_it->size;
      }
   }

   /* note: if minnode was never updated, that means no suitable element was
    *       found, so returning NULL is exactly what we want
    */
   return minnode;
}

void list_insertafter(list_node_t *new_elem, list_node_t *ref_elem, list_t *listp) {
   list_node_t *swap = ref_elem->tail;
   ref_elem->tail = new_elem;
   new_elem->tail = swap;

   /* update back of list if needed */
   if (listp->back == ref_elem) {
      listp->back = new_elem;
   }
}


void list_print(list_t *listp) {
   int i = 0;
   for (list_node_t *list_it = listp->front; list_it; list_it = list_it->tail) {
      printf("%s%p: %-10zu"COLOR_RESET"  ->  ", list_it->free ? COLOR_GREEN : COLOR_RED,
             (void *) list_it, list_it->size);
      if (i == 3) {
         printf("\n");
         i = 0;
      } else {
         ++i;
      }
   }
   printf(COLOR_RESET"\n");
}


void list_dump(FILE *stream, list_t *listp) {
   for (list_node_t *list_it = listp->front; list_it; list_it = list_it->tail) {
      fprintf(stream, "%p %c: %zu bytes\n", (void *) (list_it + 1),
              list_it->free ? '_' : 'X',
              list_it->size);
      if (!list_it->free) {
         fwrite((char *) (list_it + 1), sizeof(char), list_it->size, stream);
         fprintf(stream, "\n");
      }
      fprintf(stream, "\n");
   }
}

/* list_validate()
 * DESC:   checks that the list fulfills the following properties:
             (i)   the addresses of nodes are in ascending order
             (ii)  there is at least 1 byte of space between consecutive nodes
             (iii) there are no memory gaps between a memory block & the following
                   node
 * RETVAL: returns pointer to first node that violates above conditions; if list is
 *         valid, returns NULL
 */
list_node_t *list_validate(list_t *listp) {
   list_node_t *prev = listp->front;

   /* make sure prev is valid node */
   if (prev == NULL || prev->size == 0) {
      return prev;
   }

   /* validate all remaining nodes */
   for (list_node_t *list_it = prev->tail; list_it; prev = list_it, list_it = list_it->tail) {
         if (((char *) (prev + 1)) + prev->size != (char *) list_it) {
            return list_it;
         } else if (list_it->size == 0) {
            return list_it;
         }
   }

   return NULL;
}





void memblocks_init(list_t *memblocks) {
   memblocks->front = memblocks->back = NULL;
}

void memblock_insert(void *begin_addr, void *end_addr, list_t *memblocks) {
   list_node_t *memblock_header;
   
   /* initialize memblock header starting at begin_addr */
   memblock_header = (list_node_t *) begin_addr;
   memblock_header->size = (char *) end_addr - (char *) begin_addr - sizeof(list_node_t);
   memblock_header->free = true;

   /* append to memblocks list */
   list_append(memblock_header, memblocks);
}

list_node_t *memblock_find(size_t size, list_t *memblocks) {
   return list_minlwrbnd(size, memblocks);
}

/* memblock_split
 * DESC: splits memory block into two different blocks, the first of size
 *       _size_ and the second of the remaining size
 * NOTE: will only split the memblock if both resulting blocks will be
 *       nonzero in size
 *
 */
int memblock_split(list_node_t *block, size_t size, list_t *memblocks) {
   size_t total_size = block->size;
   size_t remaining_size;
   list_node_t *block2;
   
   if (total_size <= size + sizeof(list_node_t)) {
      /* do not split blocks */
      return false;
   }

   /* split blocks */
   remaining_size = total_size - size - sizeof(list_node_t);
   block->size = size; // update size of first block
   block2 = (list_node_t *) ((char *) (block + 1) + size);

   /* initialize new block */
   block2->size = remaining_size;
   block2->free = block->free;

   /* insert into memblocks list */
   list_insertafter(block2, block, memblocks);

   return true;
}

void memblock_free(void *begin_addr) {
   list_node_t *block = ((list_node_t *) begin_addr) - 1;
   block->free = true;
}

void memblock_allocate(void *begin_addr) {
   list_node_t *block = ((list_node_t *) begin_addr) - 1;
   block->free = false;
}
