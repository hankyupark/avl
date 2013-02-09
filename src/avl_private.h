/*-----------------------------------------------------------------------------
 * avl_private.h - private structres for the avl library
 *
 * Forhad Ahmed, June 2009 
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
 * AVL_MAX_HEIGHT: Max height of an avl tree 
 */
#define AVL_MAX_HEIGHT 48 


/*
 * AVL_DATA: Macro to get the data pointer used for avl operations based on 
 *           whether the tree is intrusive or not
 */
#define AVL_DATA(n, t) ((t->opts & AVL_INTR) ? (void *)n-t->idx*sizeof(avl_node) : (void*)n->data[0])
#define AVL_NODE(d, t) ((t->opts & AVL_INTR) ? (d-t->idx*sizeof(avl_node)) : d)


/*
 * struct avl_tree_t - Avl tree type.  
 * 
 *     Element: AVL_NODE *root
 *              Avl root node
 * 
 *     Element: AVL_COMP comp
 *              Avl compare function
 * 
 *     Element: AVL_FREE free_fn
 *              Avl node free function
 * 
 *     Element: int size
 *              Avl tree size
 * 
 *     Element: int opts
 *              Avl tree options 
 *
 *     Element: int indx
 *              Avl tree index (for multi-trees)
 */
struct avl_tree_t {
    avl_node *root;
    AVL_COMP  comp;
    AVL_FREE  free;
    int       size;
    int       opts;
    int       idx;
    int       n;
};


#endif /* AVL_PRIVATE_H_ */
