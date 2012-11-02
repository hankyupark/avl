/*-----------------------------------------------------------------------------
 * avl.c - avl tree implementation 
 * 
 * Forhad Ahmed, June 2009
 *-----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include "avl_private.h"


/*
 * Two way single rotation 
 */
#define avl_single(root, dir) do {                     \
    avl_node *save = root->child[!dir];                \
    root->child[!dir] = save->child[dir];              \
    save->child[dir] = root;                           \
    root = save;                                       \
} while (0)


/*
 * Two way double rotation 
 */
#define avl_double(root, dir) do {                     \
    avl_node *save = root->child[!dir]->child[dir];    \
    root->child[!dir]->child[dir] = save->child[!dir]; \
    save->child[!dir] = root->child[!dir];             \
    root->child[!dir] = save;                          \
    save = root->child[!dir];                          \
    root->child[!dir] = save->child[dir];              \
    save->child[dir] = root;                           \
    root = save;                                       \
} while (0)


/*
 * Adjust balance before double rotation 
 */
#define avl_adjust_balance(root, dir, bal) do {        \
    avl_node *n = root->child[dir];                    \
    avl_node *nn = n->child[!dir];                     \
    if ( nn->balance == 0 )                            \
        root->balance = n->balance = 0;                \
    else if ( nn->balance == bal ) {                   \
        root->balance = -bal;                          \
        n->balance = 0;                                \
    } else {                                           \
        root->balance = 0;                             \
        n->balance = bal;                              \
    }                                                  \
    nn->balance = 0;                                   \
} while (0)


/*
 * Rebalance after insertion 
 */
#define avl_insert_balance(root, dir) do {             \
    avl_node *n = root->child[dir];                    \
    int bal = dir == 0 ? -1 : +1;                      \
    if ( n->balance == bal ) {                         \
        root->balance = n->balance = 0;                \
        avl_single ( root, !dir );                     \
    } else {                                           \
        avl_adjust_balance ( root, dir, bal );         \
        avl_double ( root, !dir );                     \
    }                                                  \
} while (0)


/* 
 * Rebalance after deletion 
 */
#define avl_remove_balance(root, dir, done) do {       \
    avl_node *n = root->child[!dir];                   \
    int bal = dir == 0 ? -1 : +1;                      \
    if ( n->balance == -bal ) {                        \
        root->balance = n->balance = 0;                \
        avl_single ( root, dir );                      \
    }                                                  \
    else if ( n->balance == bal ) {                    \
        avl_adjust_balance ( root, !dir, -bal );       \
        avl_double ( root, dir );                      \
    } else {                                           \
        root->balance = -bal;                          \
        n->balance = bal;                              \
        avl_single ( root, dir );                      \
        done = 1;                                      \
    }                                                  \
} while (0)


/*
 * Macro to free node AND node data if required
 */
#define avl_free_node(node, tree) do {                 \
    if (tree->opts  & AVL_INTR) {                      \
        if (tree->free) tree->free(node);              \
    } else {                                           \
        if (tree->free) {                              \
            tree->free(AVL_DATA(node, tree));          \
        }                                              \
        free(node);                                    \
    }                                                  \
} while (0) 


/*
 * Create new avl node for insertion if tree is non-intrusive
 */
static avl_node *
avl_new_node(avl_tree *tree, void *data)
{
    avl_node *node;
    size_t    size = sizeof(avl_node) + sizeof(void*);

    node = (avl_node *) malloc(size);

    if (node == NULL) {
        return NULL;
    }

    node->balance = 0;
    node->data[0] = data;
    node->child[0] = node->child[1] = NULL;

    return node;
}


/*
 * avl_walk_internal() - Recursively walks the avl tree
 *
 * TODO: finish this function
 */
static int
avl_walk_internal(avl_tree      *r,
                  avl_node      *n,
                  avl_walker_fn  w,
                  void          *c,
                  int            t)
{
    if (n == NULL) {
        return AVL_SUCCESS;
    }

    switch (t) {
    case AVL_WALK_INORDER:
        if (!avl_walk_internal(r, n->child[0], w, c, t)) return AVL_ERROR;
        if (!w(AVL_DATA(n, r), c)) return AVL_ERROR;
        if (!avl_walk_internal(r, n->child[1], w, c, t)) return AVL_ERROR;
        return AVL_SUCCESS;
        break;

    case AVL_WALK_PREORDER:
        if (!w(AVL_DATA(n, r), c)) return AVL_ERROR;
        if (!avl_walk_internal(r, n->child[0], w, c, t)) return AVL_ERROR;
        if (!avl_walk_internal(r, n->child[1], w, c, t)) return AVL_ERROR;
        return AVL_SUCCESS;
        break;

    case AVL_WALK_POSTORDER:

        return AVL_SUCCESS;
        break;

    default:
        return AVL_ERROR;
    }

    return AVL_ERROR;;
}


/*
 * Create new avl tree
 */
avl_tree *
avl_new (avl_compare_fn comp_fn, avl_free_fn free_fn, int options)
{
    avl_tree *tree = (avl_tree *)malloc (sizeof(avl_tree));

    if (tree == NULL) {
        return NULL;
    }

    tree->root = NULL;
    tree->comp = comp_fn;
    tree->free = free_fn;
    tree->opts = options;
    tree->size = 0;
 
    return tree;
}


/*
 * Free an avl tree
 */
void 
avl_free(avl_tree *tree)
{
    avl_node *node = tree->root;
    avl_node *temp;

    while ( node != NULL ) {
        if (node->child[0] == NULL) {
            temp = node->child[1];
            avl_free_node(node, tree);

        } else {
            temp = node->child[0];
            node->child[0] = temp->child[1];
            temp->child[1] = node;
        }
        node = temp;
    }

    free(tree);
}


/*
 * Lookup an avl_node/user data in an avl tree
 */
avl_node *
avl_lookup(avl_tree *tree, void *data, void *ctx)
{
    avl_node *node = tree->root;
    int comp;

    while ( node != NULL ) {
        comp = tree->comp( AVL_DATA(node, tree), data, ctx );

        if (comp == 0) {
            break;
        }

        node = node->child[comp < 0];
    }

    return node;
}


/*
 * Insert an avl_node/user data in an avl tree
 */
avl_node * 
avl_insert(avl_tree *tree, void *data , void *ctx)
{
    avl_node  head = {0}; 
    avl_node *s, *t;
    avl_node *p, *q;
    avl_node *r;
    int dir;

    if (tree->root == NULL) { 
        if (tree->opts & AVL_INTR) {
            tree->root = data; 
            r = tree->root;
        } else { 
            tree->root = avl_new_node ( tree, data );
            if (tree->root == NULL) {
                return NULL;
            }
        }
    } else {
        t = &head;
        t->child[1] = tree->root;

        for (s = p = t->child[1]; ; p = q) {
            dir = tree->comp(AVL_DATA(p, tree), data, ctx) < 0;
            q = p->child[dir];

            if (q == NULL) {
                break;
            }
      
            if (q->balance != 0) {
                t = p;
                s = q;
            }
        }

        if (tree->opts & AVL_INTR) {
            q = (avl_node *) data; 
        } else {
            q = avl_new_node(tree, data);
        }

        r = q;
        p->child[dir] = q;

        if (q == NULL) {
            return NULL;
        }

        for (p = s; p != q; p = p->child[dir]) {
            dir = tree->comp(AVL_DATA(p, tree), data , ctx) < 0;
            p->balance += dir == 0 ? -1 : +1;
        }

        q = s; 

        if (abs ( s->balance ) > 1) {
            dir = tree->comp (AVL_DATA(s, tree), data, ctx ) < 0;
            avl_insert_balance ( s, dir );
        }

        if (q == head.child[1]) {
            tree->root = s;
        } else {
            t->child[q == t->child[1]] = s;
        }
    }

    ++tree->size;

    return r;
}


/*
 * Remove a node from an avl tree
 */
int 
avl_remove(avl_tree *tree, void *data , void *ctx)
{
    if (tree->root != NULL) {
        avl_node *node, *up[AVL_MAX_HEIGHT];
        int upd[AVL_MAX_HEIGHT], top = 0;
        int done = 0;

        node = tree->root;

        while (1) {
            if ( node == NULL ) {
                return 0;
            } else if ( tree->comp (AVL_DATA(node, tree), data, ctx) == 0 ) {
                break;
            }

            upd[top] = tree->comp (AVL_DATA(node, tree), data, ctx ) < 0;
            up[top++] = node;

            node = node->child[upd[top - 1]];
        }

        if (node->child[0] == NULL || node->child[1] == NULL) {
            int dir = node->child[0] == NULL;

            if ( top != 0 ) {
                up[top - 1]->child[upd[top - 1]] = node->child[dir];
            } else {
                tree->root = node->child[dir];
            }

            avl_free_node(node, tree);

        } else {
            avl_node *temp = node->child[1];
            void *save;

            upd[top] = 1;
            up[top++] = node;

            while ( temp->child[0] != NULL ) {
                upd[top] = 0;
                up[top++] = temp;
                temp = temp->child[0];
            }

            if (tree->opts & AVL_INTR) {
      
            } else {
                save = node->data[0];
                node->data[0] = temp->data[0];
                temp->data[0] = save;
            }

            up[top - 1]->child[up[top - 1] == node] = temp->child[1];

            avl_free_node(temp, tree);
        }

        while ( --top >= 0 && !done ) {
            up[top]->balance += upd[top] != 0 ? -1 : +1;
            if (abs ( up[top]->balance ) == 1) {
                break;
            } else if (abs ( up[top]->balance ) > 1) {
                avl_remove_balance ( up[top], upd[top], done );

                if ( top != 0 ) {
                    up[top - 1]->child[upd[top - 1]] = up[top];
                } else {
                    tree->root = up[0];
                }
            }
        }
        --tree->size;
    }

    return AVL_SUCCESS;
}


/*
 * Get the size of the avl tree
 */
int
avl_size(avl_tree *tree)
{
  return tree->size;
}


/*
 * Recursively calculate the height of an avl node
 */
static int
avl_height_r(avl_node *node)
{
    int left_height = 0;
    int right_height = 0;
    int max_height;
   
    if (node == NULL) {
        return 0;
    }

    if (node->child[0] != NULL) {
        left_height = avl_height_r(node->child[0]);
    }

    if (node->child[1] != NULL) {
        right_height = avl_height_r(node->child[1]);
    }
    
    max_height = left_height >= right_height ? left_height : right_height;
    
    return max_height + 1; 
}


/*
 * Calculate the height of the avl tree
 */
int
avl_height(avl_tree *tree)
{
    return avl_height_r(tree->root);
}


/*
 * Get the user data from an avl node.  Should only be called for non-intrusive
 * avl nodes (nodes that have user data hanging off them).
 */
void *
avl_data(avl_node *node)
{
    if (node != NULL) {
        return (void*)node->data[0];
    } else {
        return NULL;
    }
}


/*
 * Walk an avl tree, calling a walker function on each node.
 */
int
avl_walk(avl_tree *tree, avl_walker_fn walk, void *ctx, int type)
{
    return avl_walk_internal(tree, tree->root, walk, ctx, type);
}

