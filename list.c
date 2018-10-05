// singly linked list implementation used for mymalloc

#include "list.h"

/* list_append()
 * 
 * NOTE: list_node NEED NOT BE INITIALIZED, but space must be reserved
 */
list_t *list_append(list_node_t *nodep, list_t *listp) {
   /* if list is empty, node becomes list */
   if (listp->front == NULL) {
      listp->front = listp->back = nodep;
   } else {
      listp->back->tail = nodep;
      nodep->tail = NULL;
   }

   return listp;
}

list_t *list_prepend(list_node_t *nodep, list_t *listp) {
   nodep->tail = listp->front;
   listp->front = nodep;
   return nodep
}

inline list_t *list_insert(list_node_t *nodep, list_t *listp) {
   return list_prepend(nodep, listp);
}

/* list_minlwrbnd() -- find minimum element of list with lower bound
 * RETVAL: ptr to node if found; NULL if no such element found
 */
list_node_t *list_minlwrbnd(size_t lwrbnd, list_t *listp) {
   size_t minval;
   list_node *minnode, *list_it;

   minval = (size_t) -1; // maximum value of size_t
   minnode = NULL;
   
   /* iterate thru list */
   for (list_it = listp->front; list_it; list_it = list_it->tail) {
      /* update min val if between lower bound and current min val */
      if (list_it->val >= lwrbnd && list_it->val <= minval) {
         minnode = list_it;
         minval = list_it->val;
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

   /* update list ptrs if needed */
   if (listp->back == ref_elem) {
      listp->back = new_elem;
   }
}
