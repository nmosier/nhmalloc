#ifndef __BINTREE_H
#define __BINTREE_H

#include "memblock.h"

#define BTREE_EMPTY NULL



typedef memblock_t bnode_t;
typedef bnode_t *btree_t;

void btree_print(btree_t tree);
btree_t btree_insert(bnode_t *node, btree_t tree);
btree_t btree_collapse(bnode_t *node, bnode_t *child, btree_t tree);
btree_t btree_remove(bnode_t *node, btree_t tree);
bnode_t *btree_minlwrbnd(size_t size, btree_t tree);
const bnode_t *btree_validate(const btree_t tree);
int btree_errno();
const char *btree_strerror(int errno);
void btree_perror(const char *prefix);
#endif
