/*-----------------------------------------------------------------------------
 * avl_private.h - private structres for the avl library
 *
 * Forhad Ahmed, January 2013 
 *-----------------------------------------------------------------------------
 */

#ifndef _AVL_PRIVATE_H_
#define _AVL_PRIVATE_H_

#include "avl.h"


/*
 * Macros for source compaction
 */
#define AVL_COMP avl_compare_fn
#define AVL_FREE avl_free_fn
#define AVL_INTR AVL_TREE_INTRUSIVE

 
/*
 * Macro to get the data pointer used for avl operations based on 
 * whether the tree is intrusive or not
 */
#define AVL_DATA(n, t) ((t->opts & AVL_INTR) ? (void *)n : (void*)n->data[0])

/*
 * Macro for AVL node balance
 */
#define AVL_BALANCED(n) (n->balance < 0)


/*
 * struct avl_tree_t - Avl tree type.  
 */
struct avl_tree_t {
    avl_node *root;
    AVL_COMP  comp;
    AVL_FREE  free;
    int       size;
    int       opts;
};

#endif /* AVL_PRIVATE_H_ */

