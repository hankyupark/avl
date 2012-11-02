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
#define AVL_NODE struct avl_node_t
#define AVL_COMP avl_compare_fn
#define AVL_FREE avl_free_fn
#define AVL_INTR AVL_TREE_INTRUSIVE


/*
 * AVL_MAX_HEIGHT: Max height of an avl tree 
 */
#define AVL_MAX_HEIGHT 64 


/*
 * AVL_DATA: Macro to get the data pointer used for avl operations based on 
 *           whether the tree is intrusive or not
 */
#define AVL_DATA(n, t) ((t->opts & AVL_INTR) ? (void *)n : (void*)n->data[0])


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
 */
struct avl_tree_t {
    AVL_NODE *root;
    AVL_COMP  comp;
    AVL_FREE  free;
    int       size;
    int       opts;
};


#endif /* AVL_PRIVATE_H_ */
