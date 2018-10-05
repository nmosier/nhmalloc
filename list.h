#include <stdio.h>

typedef struct list_node_t_ {
   size_t size;     // size of memory block (not including struct header)
   int free;        // is the block free or mallocated?
   struct list_node_t_ *tail; // next memory block in heap (ptr to header)
} list_node_t;

typedef struct {
   list_node_t *front;
   list_node_t *back;
} list_t;


void list_append(list_node_t *nodep, list_t *listp);
void list_prepend(list_node_t *nodep, list_t *listp);
void list_insert(list_node_t *nodep, list_t *listp);
list_node_t *list_minlwrbnd(size_t lwrbnd, list_t *listp);
void list_insertafter(list_node_t *new_elem, list_node_t *ref_elem, list_t *listp);

void list_print(list_t *listp);
void list_dump(FILE *stream, list_t *listp);
list_node_t *list_validate(list_t *listp);
