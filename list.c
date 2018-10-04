typedef size_t list_value_t;

typedef struct {
   list_value_t val;
   list_node *tail;
} list_node_t;

typedef list_node_t list_t;
#define LIST_EMPTY ((list_node *) NULL)

// function prototypes
list_t *list_append(list_node_t *nodep, list_t *listp);
list_t *list_prepend(list_node_t *nodep, list_t *listp);
list_t *list_insert(list_node_t *nodep, list_t *listp);


// function implementations

/* list_append()
 * 
 * NOTE: list_node NEED NOT BE INITIALIZED, but space must be reserved
 */
list_t *list_append(list_node_t *nodep, list_t *listp) {
   /* if list is empty, node becomes list */
   if (listp == LIST_EMPTY) {
      nodep->tail = LIST_EMPTY;
      return nodep;
   }

   list_node_t *list_it;

   /* find end of list */
   for (list_it = (list_node_t *) listp; list_it->tail; list_it = list_it->tail) {}

   /* append node */
   list_it->tail = nodep;

   /* partially initialize node */
   nodep->tail = LIST_EMPTY;

   return listp;
}

list_t *list_prepend(list_node_t *nodep, list_t *listp) {
   nodep->tail = listp;
   return nodep
}

inline list_t *list_insert(list_node_t *nodep, list_t *listp) {
   return list_prepend(nodep, listp);
}
