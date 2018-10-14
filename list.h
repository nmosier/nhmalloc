#ifndef __LIST_H
#define __LIST_H

#include <stdio.h>
#include "memblock.h"

typedef memblock_t list_node_t;
typedef memblocks_t list_t;

void list_append(list_node_t *nodep, list_t *listp);
//void list_prepend(list_node_t *nodep, list_t *listp);
void list_insert(list_node_t *nodep, list_t *listp);

void list_print(list_t *listp);
//list_node_t *list_validate(list_t *listp);

#endif
