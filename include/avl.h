/*-----------------------------------------------------------------------------
 * avl.h - AVL tree implementation. 
 *
 * Forhad Ahmed, June 2009
 *-----------------------------------------------------------------------------
 */

#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_


/*
 * struct avl_node_t - Avl node type.  If an avl tree is intrusive, this must
 * be the first element in the user data type, and for this reason, we have to 
 * make this type transparent.
 * 
 *     Element: int balance
 *              Node balance factor
 * 
 *     Element: avl_node *child[2]
 *              Left (0) and right (1) pointers
 * 
 *     Element: void *data[0]
 *              Variable length array to contain either user data pointer if 
 *              tree is non-intrusive OR nothing (zero sized array) if tree
 *              is intrusive.
 */
typedef struct avl_node_t avl_node;

struct avl_node_t {
    int       balance;   
    avl_node *child[2];
    void     *data[0];  
};


/*
 * Opaque type for an avl tree.
 */
typedef struct avl_tree_t avl_tree;


/*
 * AVL tree options - Passed to avl_new_tree().
 * 
 *     AVL_TREE_DEFAULT:   Default option (non intrusive) == NULL
 * 
 *     AVL_TREE_INTRUSIVE: User data does not hang off avl_node types. Instead
 *                         user data types are expected to have an avl_node 
 *                         element as the first element in their structures.                             
 */
#define AVL_TREE_DEFAULT   0x00000000 
#define AVL_TREE_INTRUSIVE 0x00000001


/*
 * avl_compare_fn() - Comparison function template for comparing two avl nodes.
 * If the avl nodes are intrusive, then the data pointers are avl_node types; 
 * otherwise, they are the data fields contained within avl_node type. Same is
 * true for all other avl api's that pass a void * for data.
 *    
 *     Argument: void *a
 *          IN   Data to compare
 *   
 *     Argument: void *b
 *          IN   Data to compare
 * 
 *     Argument: void *ctx
 *          IN   Context used for comparison 
 *
 *       Return: int
 *               1 if a > b, -1 if a < b, 0 if a == b.
 */
typedef int (*avl_compare_fn) (void *a, void *b, void *ctx);


/*
 * avl_walker_fn() - Walker function template for walking an avl tree.
 * 
 *     Argument: void *n
 *          IN   Avl node or user data in an avl tree
 * 
 *     Argument: void *ctx
 *          IN   Context used for the walker function
 *
 *       Return: int
 *               On success, AVL_SUCCESS == 1
 *               On failure, AVL_ERROR
 */
typedef int (*avl_walker_fn) (void *n, void *ctx);


/*
 * avl_free_fn() - Free function called for every avl node when the tree is 
 * being destroyed.
 * 
 *     Argument: void *n
 *          IN   Avl node or user data to free
 */
typedef void (*avl_free_fn) (void *n);


/*
 * AVL walk types - Passed to avl_walk().  Self-explanatory.
 */
enum {
    AVL_WALK_INORDER,
    AVL_WALK_PREORDER,
    AVL_WALK_POSTORDER
};


/*
 * Generic return values 
 */
enum {
    AVL_ERROR = 0,
    AVL_SUCCESS = 1
};


/*
 * avl_new() - Create a new avl tree.  
 *     
 *     Argument: avl_compare_fn comp
 *          IN   Comparison function used for ins/del operations for this tree
 *
 *     Argument: avl_free_fn free_fn
 *          IN   Free function called for each avl_node  when avl tree is being 
 *               destroyed.  If NULL, default libc free() is used. 
 *
 *     Argument: int options
 *          IN   Option bits for this avl tree.  
 *
 *       Return: avl_tree *
 *               Newly created tree or NULL if error (comparison fn is null or
 *               memory error)
 */
avl_tree *
avl_new(avl_compare_fn comp, avl_free_fn free_fn, int options);


/*
 * avl_free() - Free an avl tree.
 * 
 *     Argument: avl_tree *
 *          IN   Tree to destroy.
 */
void 
avl_free(avl_tree *tree);


/*
 * avl_insert() - Insert an avl_node/user data into an avl tree.
 * 
 *     Argument: avl_tree *tree
 *          IN   Avl tree to insert into
 *  
 *     Argument: void *data
 *          IN   Avl node or user data to insert into tree.
 * 
 *     Argument: void *ctx
 *          IN   Context used for compare operations during insert
 */
avl_node *
avl_insert(avl_tree *tree, void *data, void *ctx);


/*
 * avl_remove() - remove an avl_node/user data from an avl tree.
 * 
 *     Argument: avl_tree *tree
 *          IN   Avl tree to remove from
 *    
 *     Argument: void *data
 *          IN   Avl node or user data to remove from tree.
 * 
 *     Argument: void *ctx
 *          IN   Context used for compare operations during remove
 */
int 
avl_remove(avl_tree *tree, void *data, void *ctx);


/*
 * avl_size() - Get the size of an avl tree
 * 
 *     Argument: avl_tree *tree
 *         IN    Avl tree 
 * 
 *       Return: int
 *               Size of the tree
 */
int
avl_size(avl_tree *tree);


/*
 * avl_data() - Get the user data associated with an avl node.  Used only for
 * avl trees that are non-intrusive (user data hangs off avl nodes).
 * 
 *     Argument: avl_node *node
 *         IN    Avl node to get user data from
 * 
 *       Return: void *
 *               User data associated with avl node
 */
void *
avl_data(avl_node *node);


/*
 * avl_lookup() - Lookup an avl_node/user data in an avl tree.
 * 
 *     Argument: avl_tree *tree
 *          IN   Avl tree to lookup
 * 
 *     Argument: void *data 
 *          IN   Avl node or user data to lookup in tree
 * 
 *     Argument: void *ctx
 *          IN   Context used for compare operation during lookup
 * 
 *       Return: avl_node *
 *               Avl node matching the lookup if lookup succeeds; o/w NULL.  If
 *               the tree nodes are non-intrusive, then use avl_data() to get 
 *               the user data associated with the avl_node returned.
 */
avl_node *
avl_lookup(avl_tree *tree, void *data, void *ctx);


/*
 * avl_walk() - Recursive walk of an avl tree
 *  
 *     Argument: avl_tree *tree 
 *          IN   Avl tree to walk
 *    
 *     Argument: avl_walker_fn walk
 *          IN   Walker function called for every avl node / user data
 * 
 *     Argument: void *ctx
 *          IN   Context passed to the walker function for each node walked
 *
 *     Argument: int type
 *          IN   Walk type (AVL_*_WALK)
 * 
 *       Return: int
 *               On success, AVL_SUCCESS == 1
 *               On failure, AVL_ERROR
 */
int
avl_walk(avl_tree *tree, avl_walker_fn walk, void *ctx, int type);


#endif /* _AVL_TREE_H_ */

