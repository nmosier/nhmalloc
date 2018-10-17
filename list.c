// singly linked list implementation used for mymalloc
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "list.h"
#include "memblock.h"
#include "debug.h"

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
         if (tail != nodep) {
            eprintf("list_append: appended node at unexpected address %p (expected %p)\n",
                    (void *) nodep, (void *) tail);
            LOG("dumping list...\n");
            list_print(listp);
            exit(1);
         }
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
      nodep->prevp = tail->prevp;
      tail->prevp = nodep;
   }
}


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



typedef enum list_e_t_ {
   LIST_E_SUCCESS,
   LIST_E_PREV,
   LIST_E_SIZE
} list_e_t;

static list_e_t list_errno_;

/* list_validate()
 * DESC:   checks that the list fulfills the following properties:
             (i)   the addresses of nodes are in ascending order
             (ii)  there is at least 1 byte of space between consecutive nodes
             (iii) there are no memory gaps between a memory block & the following
                   node
 * RETVAL: returns pointer to first node that violates above conditions; if list is
 *         valid, returns NULL
 */
const list_node_t *list_validate(const list_t *listp) {
   const list_node_t *list_it, *back, *prevp;

   list_errno_ = LIST_E_SUCCESS;
   
   /* prime the loop */
   back = listp->back;
   list_it = listp->front;
   prevp = NULL;

   if (list_it == NULL) {
      return NULL;
   }
   if (prevp != list_it->prevp) {
      list_errno_ = LIST_E_PREV;
      return list_it;
   }
   if (list_it->size == 0) {
      list_errno_ = LIST_E_SIZE;
      return list_it;
   }
   prevp = list_it;
   do {
      list_it = (list_node_t *) ((char *) list_it + list_it->size + sizeof(list_node_t));
      if (prevp != list_it->prevp) {
         list_errno_ = LIST_E_PREV;
         return list_it;
      }
      if (list_it->size == 0) {
         list_errno_ = LIST_E_SIZE;
         return list_it;
      }
      prevp = list_it;
   } while (list_it != back);

   return NULL;
}

int list_errno() {
   return (int) list_errno_;
}

const char *list_strerror(int errno) {
   switch ((list_e_t) errno) {
   case LIST_E_SUCCESS: return "Success";
   case LIST_E_PREV:    return "Previous node mismatch";
   case LIST_E_SIZE:    return "Block is of size 0";
   default:             return "Unknown error";
   }
}

void list_perror(const char *prefix) {
   eprintf("%s: %s\n", prefix, list_strerror(list_errno_));
}
