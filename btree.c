/* bintree.c
 * implementation of binary tree and related functions
 * custom written for malloc
 */

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "btree.h"

#define LOG(str) write(STDERR_FILENO, str, strlen(str))

char eprintf_buf[1000];
#define eprintf(fmt, ...) sprintf(eprintf_buf, fmt, __VA_ARGS__),   \
   LOG(eprintf_buf)


// colors
#define COLOR_RED   "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

btree_t btree_insert(bnode_t *node, btree_t tree) {
   size_t newsize = node->size;
   
   if (tree == BTREE_EMPTY) {
      node->leftp = node->rightp = node->parentp = NULL;
      return node;
   }

   bnode_t *curnode = (bnode_t *) tree;
   bnode_t **curnode_childp = NULL; // pointer to child pointer in parent node
   while (curnode_childp == NULL) {
      if (newsize < curnode->size) {
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

   return tree;
}

/* btree_collapse()
 * DESC: "collapses" edges around _node_, i.e. removing node from tree.
 *       _node_'s _dir_ child (0=left, 1=right) is updated
 *       as _node_'s parent's _dir_ child
 * PARAMS:
 *  - node: node to collapse tree around
 * RETURN: new tree
 * NOTE: tree should be non-NULL
 */
btree_t btree_collapse(bnode_t *node, bnode_t *child, btree_t tree) {
   bnode_t *parentp;
   
   if (node == tree) {
      if (child) {
         child->parentp = NULL;
      }
      return child;
   }

   parentp = node->parentp;
   if (parentp->leftp == node) {
      parentp->leftp = child;
   } else {
      parentp->rightp = child;
   }

   if (child) {
      child->parentp = parentp;
   }

   return tree;
}

/* btree_remove()
 * DESC: remove node from tree
 * NOTE: neither _node_ nor _tree_ should ever be NULL upon entry.
 */
btree_t btree_remove(bnode_t *node, btree_t tree) {
   bnode_t *rnode, *pnode, *leftch, *rightch, **parent2child;

   /* find _node_'s relation to parent */
   pnode = node->parentp;
   if (pnode) {
      parent2child = (pnode->leftp == node) ? &pnode->leftp : &pnode->rightp;
   }

   if (node->leftp) {
      /* find suitable node to replace _node_;
       * must be greatest size sz such that
       * sz < _node_->size */
      for (rnode = node->leftp; rnode->rightp; rnode = rnode->rightp) {}

      /* collapse _rnode_ from tree */
      tree = btree_collapse(rnode, rnode->leftp, tree);
      
      /* replace _node_ with _rnode_ in tree */
      if (pnode) {
         *parent2child = rnode;
      }
      leftch = node->leftp;
      rightch = node->rightp;
      
      /* update _node_'s children */
      if (leftch) {
         leftch->parentp = rnode;
      }
      if (rightch) {
         rightch->parentp = rnode;
      }
      rnode->leftp = leftch;
      rnode->rightp = rightch;
   } else {
      /* since _node_ has no left child, just collapse
       * it out of the tree
       */
      tree = btree_collapse(node, node->rightp, tree);
   }
   
   return tree;
}

bnode_t *btree_minlwrbnd(size_t size, btree_t tree) {
   bnode_t *curnode, *minnode;
   size_t minval, curval;

   curnode = (bnode_t *) tree;
   minnode = NULL;
   minval = (size_t) -1;
   while (curnode) {
      curval = curnode->size;
      if (curval <= minval && curval >= size) {
         minnode = curnode;
         minval = curval;
      }

      if (size < curval) {
         curnode = curnode->leftp;
      } else {
         curnode = curnode->rightp;
      }
   }

   return minnode;
}

// btree_print's auxiliary functions
bnode_t *btree_print_row(bnode_t *node);
int btree_depth_right(btree_t tree);
void btree_print_aux(btree_t tree, const char *prefix, btree_t term);

void btree_print(btree_t tree) {
   eprintf("tree@%p\n", (void *) tree);
   
   const char *prefix = "";
   btree_print_aux(tree, prefix, NULL);
}


#define ENTRY_WIDTH 20
void btree_print_aux(btree_t tree, const char *prefix, btree_t term) {
   bnode_t *last_node;
   int depth = btree_depth_right(tree);
   size_t prefix_len = strlen(prefix);
   char padding[ENTRY_WIDTH * depth + prefix_len + 1];
   char *padp = padding;
   
   /* print right nodes */
   eprintf("%s", prefix);
   last_node = btree_print_row(tree);
   memcpy(padp, prefix, prefix_len);
   padp += prefix_len;
   memset(padp, ' ', depth * ENTRY_WIDTH);
   for (bnode_t *edge_it = tree; edge_it; edge_it = edge_it->rightp) {
      padp[ENTRY_WIDTH/2] = edge_it->leftp ? '|' : ' ';
      padp += ENTRY_WIDTH;
   }
   *padp = '\0';
      
   /* now recursively print */
   for (bnode_t *rev_it = last_node; rev_it && rev_it != term; rev_it = rev_it->parentp) {
      if (rev_it->leftp) {
         /* put in extra line of padding for asthetics */
         eprintf("%s\n", padding);
         eprintf("%s\n", padding);
         
         padp -= ENTRY_WIDTH;
         *padp = '\0';
         
         btree_print_aux(rev_it->leftp, padding, rev_it);
      } else {
         padp -= ENTRY_WIDTH;
         *padp = '\0';
      }
   }
}

// returns last node
bnode_t *btree_print_row(bnode_t *node) {
   bnode_t *prev = NULL;
   const char *stredge = "----";
   const char *strnoedge = "    ";
   char sbuf[1000], snode[100];

   for (bnode_t *b_it = node; b_it; b_it = b_it->rightp) {
      sprintf(snode, "%zu @ %p", b_it->size, (void *) b_it);
      sprintf(sbuf, "%*.*s%s", ENTRY_WIDTH - (int) strlen(stredge),
              ENTRY_WIDTH - (int) strlen(stredge), snode,
              b_it->rightp ? stredge : strnoedge);
      write(STDERR_FILENO, sbuf, strlen(sbuf));
      prev = b_it;
   }
   LOG("\n");
   
   return prev;
}


int btree_depth_right(btree_t tree) {
   int depth = 0;
   for (bnode_t *node = (bnode_t *) tree; node; node = node->rightp) {
      ++depth;
   }

   return depth;
}


int btree_validate_cmp(const void *lhs, const void *rhs);
size_t btree_nodecount(bnode_t *root);
size_t btree_array(bnode_t *root, bnode_t **arr);
/* btree_validate()
 * DESC: checks to make sure there are no memory gaps
 *       between the memory blocks in the tree
 * PARAMS:
 */
bnode_t *btree_validate(btree_t tree) {
   size_t addrs_len = btree_nodecount(tree);
   bnode_t *addrs[addrs_len], *errblk;
   
   
   /* get array of nodes */
   if (btree_array(tree, addrs) != addrs_len) {
      LOG("btree_validate: internal error.\n");
      exit(1);
   }

   /* sort array of nodes by address */
   qsort(addrs, addrs_len, sizeof(bnode_t *), btree_validate_cmp);

   /* compare address gaps against block sizes */
   errblk = NULL;
   for (bnode_t **addr_it = addrs; addr_it != addrs + addrs_len - 1; ++addr_it) {
      bnode_t *cur_addr = addr_it[0], *next_addr = addr_it[1];
      if (cur_addr->size + sizeof(bnode_t) != (char *) next_addr - (char *) cur_addr) {
         errblk = cur_addr;
         break;
      }
   }

   if (errblk) {
      eprintf("memblocks_validate: validation "COLOR_RED\
              "FAILED"COLOR_RESET" for node %p in\n",
              (void *) errblk);
      /* print sorted list */
      for (int i = 0; i < addrs_len; ++i) {
         eprintf("%p\t", (void *) addrs[i]);
      }
      LOG("\n");
      btree_print(tree);
   } else {
      eprintf("memblocks_validate: validation "COLOR_GREEN\
              "succeeded"COLOR_RESET".%s", "\n"); // requires ≥1 variadic param
   }

   
   return errblk;
}

int btree_validate_cmp(const void *lhs, const void *rhs) {
   bnode_t *lhsp, *rhsp;
   lhsp = *((bnode_t **) lhs);
   rhsp = *((bnode_t **) rhs);
   return (char *) lhsp - (char *) rhsp;
}

size_t btree_nodecount(bnode_t *root) {
   if (root) {
      return btree_nodecount(root->leftp) + btree_nodecount(root->rightp) + 1;
   } else {
      return 0;
   }
}

/* btree_array()
 * DESC: writes addresses of nodes to array
 * RETVAL: returns # of addresses written
 */
size_t btree_array(bnode_t *root, bnode_t **arr) {
   if (root) {
      size_t count = 1;
      *arr = root;
      count += btree_array(root->leftp, arr + count);
      count += btree_array(root->rightp, arr + count);
      return count;
   } else {
      return 0;
   }
}
