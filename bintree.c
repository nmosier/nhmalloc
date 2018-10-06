/* bintree.c
 * implementation of binary tree and related functions
 * custom written for malloc
 */

typedef struct bnode_t_ {
   struct bnode_t_ *leftp;
   struct bnode_t_ *rightp;
   struct bnode_t_ *parentp;
   size_t val;
} bnode_t;

typedef bnode_t btree_t;

#define BTREE_EMPTY NULL

btree_t *btree_insert(const bnode_t *node, btree_t *tree) {
   size_t newval = node->val;
   
   if (tree == BTREE_EMPTY) {
      return (btree_t *) node;
   }

   bnode_t *curnode = (bnode_t *) tree;
   bnode_t **curnode_childp = NULL; // pointer to child pointer in parent node
   while (curnode_childp == NULL) {
      if (newval < curnode->val) {
         if (curnode->leftp) {
            curnode = curnode->leftp;
         } else {
            curnode_childp = &curnode->leftp;
         }
      } else {
         if (curnode->rightp) {
            curnode = curnode->rightp;
         } else {
            curnode_childp = &curnode->rightp;
         }
      }
   }

   *curnode_childp = node;
   node->parentp = curnode;
   node->leftp = node->rightp = NULL;
}



bnode_t *btree_minlwrbnd(size_t val, btree_t *tree) {
   bnode_t *curnode, *minnode;
   size_t minval, curval;

   curnode = (bnode_t *) tree;
   minnode = NULL;
   minval = (size_t) -1;
   while (curnode) {
      curval = curnode->val;
      if (curval <= minval && curval >= val) {
         minnode = curnode;
         minval = curval;
      }

      if (val < curval) {
         curnode = curnode->leftp;
      } else {
         curnode = curnode->rightp;
      }
   }

   return minnode;
}

