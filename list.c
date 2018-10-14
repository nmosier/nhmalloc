// singly linked list implementation used for mymalloc
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "memblock.h"

#define COLOR_RED   "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"


#define LOG(str) write(STDERR_FILENO, str, strlen(str))

char eprintf_buf[1000];
#define eprintf(fmt, ...) sprintf(eprintf_buf, fmt, __VA_ARGS__),   \
   LOG(eprintf_buf)


#define DEBUG 1

/* list_append()
 * 
 * NOTE: list_node NEED NOT BE INITIALIZED, but space must be reserved
 */
void list_append(list_node_t *nodep, list_t *listp) {
   /* if list is empty, node is also front */
   if (listp->front == NULL) {
      listp->front = listp->back = nodep;
   } else {
      list_node_t *back, *tail;
      back = listp->back;

      if (DEBUG) {
         tail = (list_node_t *) ((char *) back + back->size + sizeof(list_node_t));
         assert(tail == nodep);
      }

      nodep->prevp = back;
      listp->back = nodep;
   }
}

// never actually used
/*
void list_prepend(list_node_t *nodep, list_t *listp) {
   nodep->tail = listp->front;
   listp->front = nodep;
}
*/


void list_insert(list_node_t *nodep, list_t *listp) {
   if (nodep > listp->back) {
      /* append to list */
      list_append(nodep, listp);
   } else {
      /* insert somewhere in middle of list */
      list_node_t *tail = (list_node_t *) ((char *) nodep + nodep->size + sizeof(list_node_t));
      tail->prevp = nodep;
   }
}


/*
void list_insertafter(list_node_t *new_elem, list_node_t *ref_elem, list_t *listp) {


   
   list_node_t *swap = ref_elem->tail;
   ref_elem->tail = new_elem;
   new_elem->tail = swap;

   // update back of list if needed
   if (listp->back == ref_elem) {
      listp->back = new_elem;
   }
}
*/


void list_print(list_t *listp) {
   list_node_t *list_it;
   int i = 0;

   eprintf("list @ %p\n", (void *) listp->front);
   
   /* prime the loop */
   list_it = listp->front;
   if (list_it == NULL) {
      LOG("(nil)\n");
      return;
   }
   eprintf("%s%p: %-10zu"COLOR_RESET"  ->  ",
          list_it->free ? COLOR_GREEN : COLOR_RED,
          (void *) list_it, list_it->size);
   ++i;
   do {
      list_it = (list_node_t *) ((char *) list_it + list_it->size + sizeof(list_node_t));
      eprintf("%s%p: %-10zu"COLOR_RESET"  ->  ",
             list_it->free ? COLOR_GREEN : COLOR_RED,
             (void *) list_it, list_it->size);
      if (i % 3 == 0) {
         LOG("\n");
      }
      ++i;
   } while (list_it != listp->back);

   LOG(COLOR_RESET"\n");
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

// TO BE FIXED
/*
list_node_t *list_validate(list_t *listp) {
   list_node_t *prev = listp->front;

   // make sure prev is valid node
   if (prev == NULL || prev->size == 0) {
      return prev;
   }

   // validate all remaining nodes
   for (list_node_t *list_it = prev->tail; list_it; prev = list_it, list_it = list_it->tail) {
         if (((char *) (prev + 1)) + prev->size != (char *) list_it) {
            return list_it;
         } else if (list_it->size == 0) {
            return list_it;
         }
   }

   return NULL;
}
*/
