// singly linked list implementation used for mymalloc
#include <stdio.h>

#include "list.h"


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
