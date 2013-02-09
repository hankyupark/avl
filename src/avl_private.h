/*-----------------------------------------------------------------------------
 * avl_private.h - private structres for the avl library
 *
 * Forhad Ahmed, June 2009 
 *-----------------------------------------------------------------------------
 */

#ifndef _AVL_PRIVATE_H_
#define _AVL_PRIVATE_H_

/*
 * Macros for source compaction
 */
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


#endif /* AVL_PRIVATE_H_ */
