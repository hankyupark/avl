#include <stdlib.h>
#include "avl_private.h"

const avl_node AVL_INODE = {{0,0},-1};

static avl_node *
avl_new_node(avl_tree *tree, void *data)
{
    avl_node *node;
    size_t    size = sizeof(avl_node) + sizeof(void*);

    node = (avl_node *) malloc(size);

    if (node == NULL) return NULL;
    
    node->balance = AVL_NONE;
    node->data[0] = data;
    node->child[0] = node->child[1] = NULL;

    return node;
}

static inline void
avl_free_node(avl_tree *tree, avl_node *node)
{
    if (tree->opts & AVL_INTR) {
        if (tree->free) tree->free(node);
    } else {
        if (tree->free) tree->free(AVL_DATA(node, tree));
        free(node);
    }
}

static avl_node *
avl_rotate_2(avl_node **top, int dir)
{
    avl_node *B, *C, *D, *E;
    B = *top;
    D = B->child[dir];
    C = D->child[1-dir];
    E = D->child[dir];

    *top = D;
    D->child[1-dir] = B;
    B->child[dir] = C;
    B->balance = AVL_NONE;
    D->balance = AVL_NONE;
    return E;
}

static avl_node *
avl_rotate_3(avl_node **top, int dir, int third)
{
    avl_node *B, *F, *D, *C, *E;
    B = *top;
    F = B->child[dir];
    D = F->child[1-dir];
    C = D->child[1-dir];
    E = D->child[dir];
    *top = D;
    D->child[1-dir] = B;
    D->child[dir] = F;
    B->child[dir] = C;
    F->child[1-dir] = E;
    D->balance = AVL_NONE;
    B->balance = F->balance = AVL_NONE;

    if (third == AVL_NONE)
        return NULL;
    else if (third == dir) {
        B->balance = 1-dir;
        return E;
    } else {
        F->balance = dir;
        return C;
    }
}
 
static inline void 
avl_rebalance_path(avl_tree *tree, avl_node *node, void *data, void *ctx)
{
    while (node && tree->comp(data, AVL_DATA(node, tree), ctx)) {
        int step = (tree->comp(data, AVL_DATA(node, tree), ctx) > 0);
        node->balance = step;
        node = node->child[step];
    }
}

static inline void 
avl_rebalance_insert(avl_tree *tree, avl_node **top, void *data, void *ctx)
{
    avl_node *node = *top;
    int first, second, third;
    if (AVL_BALANCED(node));
    else if (node->balance != (first = (tree->comp(data, AVL_DATA(node, tree), ctx) > 0))) {
        node->balance = AVL_NONE;
        node = node->child[first]; 
    } else if (first == (second = (tree->comp(data, AVL_DATA(node->child[first], tree), ctx) > 0))) {
        node = avl_rotate_2(top, first);
    } else {
        node = node->child[first]->child[second];
        if (tree->comp(data, AVL_DATA(node, tree), ctx) == 0) third = AVL_NONE;
        else third = (tree->comp(data, AVL_DATA(node, tree), ctx) > 0);
        node = avl_rotate_3(top, first, third);
    }
    avl_rebalance_path(tree, node, data, ctx);
}

static inline void 
avl_swap_del(avl_tree *tree, avl_node **ptarget, avl_node **proot, int dir)
{
    avl_node *target = *ptarget;
    avl_node *node = *proot;

    *ptarget = node;
    *proot = node->child[1-dir];
    node->child[AVL_LEFT] = target->child[AVL_LEFT];
    node->child[AVL_RIGHT]= target->child[AVL_RIGHT];
    node->balance = target->balance;
    avl_free_node(tree, target);
}

static inline avl_node **
avl_rebalance_del(avl_tree *tree, avl_node **top, avl_node **ptarget, void *data, void *ctx)
{
    avl_node *target = *ptarget;

    while (1) {
        avl_node *node = *top;
        int dir = (tree->comp(data, AVL_DATA(node, tree), ctx) > 0);
        if (node->child[dir]==NULL) break;
        if (AVL_BALANCED(node)) {
            node->balance = 1-dir;
        } else if (node->balance == dir) {
            node->balance = AVL_NONE;
        } else {
            int second = node->child[1-dir]->balance;
            if (second == dir) {
                avl_rotate_3(top, 1-dir, node->child[1-dir]->child[dir]->balance);
            } else if (second == AVL_NONE) {
                avl_rotate_2(top, 1-dir);
                node->balance = 1-dir;
                (*top)->balance = dir;
            } else {
                avl_rotate_2(top, 1-dir);
            }
            if (node == target) {
                ptarget = &(*top)->child[dir];
            }
        }
        top = &node->child[dir];
    }
    return ptarget;
}
 
static int
avl_walk_r(avl_tree *r, avl_node *n, avl_walker_fn w, void *c, int t)
{
    if (n == NULL) {
        return AVL_SUCCESS;
    }

    switch (t) {
    case AVL_WALK_INORDER:
        if (!avl_walk_r(r, n->child[0], w, c, t)) return AVL_ERROR;
        if (!w(AVL_DATA(n, r), c)) return AVL_ERROR;
        if (!avl_walk_r(r, n->child[1], w, c, t)) return AVL_ERROR;
        return AVL_SUCCESS;
        break;

    case AVL_WALK_PREORDER:
        if (!w(AVL_DATA(n, r), c)) return AVL_ERROR;
        if (!avl_walk_r(r, n->child[0], w, c, t)) return AVL_ERROR;
        if (!avl_walk_r(r, n->child[1], w, c, t)) return AVL_ERROR;
        return AVL_SUCCESS;
        break;

    case AVL_WALK_POSTORDER:
        // TODO
        return AVL_SUCCESS;
        break;

    default:
        return AVL_ERROR;
    }

    return AVL_ERROR;;
}

static int
avl_height_r(avl_node *node)
{
    int left_height = 0, right_height = 0,max_height;
   
    if (node == NULL) return 0;
    if (node->child[0] != NULL) left_height = avl_height_r(node->child[0]);
    if (node->child[1] != NULL) right_height = avl_height_r(node->child[1]);
    max_height = left_height >= right_height ? left_height : right_height;
    return max_height + 1; 
}

avl_tree *
avl_new (avl_compare_fn comp_fn, avl_free_fn free_fn, int options)
{
    avl_tree *tree = (avl_tree *)malloc(sizeof(avl_tree));

    if (tree == NULL) return NULL;
  
    tree->root = NULL;
    tree->comp = comp_fn;
    tree->free = free_fn;
    tree->opts = options;
    tree->size = 0;
 
    return tree;
}

void 
avl_free(avl_tree *tree)
{
    avl_node *node = tree->root, *temp;

    while ( node != NULL ) {
        if (node->child[0] == NULL) {
            temp = node->child[1];
            avl_free_node(tree, node);

        } else {
            temp = node->child[0];
            node->child[0] = temp->child[1];
            temp->child[1] = node;
        }
        node = temp;
    }

    free(tree);
}

void *
avl_data(avl_node *node)
{
    if (node != NULL) return (void*)node->data[0];
    else return NULL;
}

avl_node *
avl_lookup(avl_tree *tree, void *data, void *ctx)
{
    avl_node *node = tree->root;

    while (node && tree->comp(data, AVL_DATA(node, tree), ctx)) {
        int step = (tree->comp(data, AVL_DATA(node, tree), ctx) > 0);
        node = node->child[step];
    }
    return node;
}

int 
avl_insert(avl_tree *tree, void *data, void *ctx)
{
    avl_node **proot = &tree->root;
    avl_node *node = *proot;
    avl_node **top = proot;
    while (node && tree->comp(data, AVL_DATA(node, tree), ctx)) {
        int step = (tree->comp(data, AVL_DATA(node, tree), ctx) > 0);
        if (!AVL_BALANCED(node)) top = proot;
        proot = &node->child[step];
        node = *proot;
    }
    if (node) return 0;

    if (tree->opts & AVL_INTR) {
        node = (avl_node *) data;
    } else {
        node = avl_new_node(tree, data);
    }

    *proot = node;
    avl_rebalance_insert(tree, top, data, ctx);
    tree->size++;
    return 1;
}

int 
avl_remove(avl_tree *tree, void *data, void *ctx)
{
    avl_node **proot = &tree->root;
    avl_node  *node = *proot;
    avl_node **top = proot;
    avl_node **ptarget = NULL;
    int dir;
    
    while (node) {
        dir = (tree->comp(data, AVL_DATA(node, tree), ctx) > 0);
        if (tree->comp(data, AVL_DATA(node, tree), ctx) == 0)
            ptarget = proot;
        if (node->child[dir] == NULL)
            break;
        if (AVL_BALANCED(node) || (node->balance == (1-dir) && AVL_BALANCED(node->child[1-dir]))) top = proot;
        proot = &node->child[dir];
        node = *proot;
    }
    if (!ptarget) return 0;

    ptarget = avl_rebalance_del(tree, top, ptarget, data, ctx);
    avl_swap_del(tree, ptarget, proot, dir);
    tree->size--;
    return 1;
}

int
avl_walk(avl_tree *tree, avl_walker_fn walk, void *ctx, int type)
{
    return avl_walk_r(tree, tree->root, walk, ctx, type);
}

int
avl_height(avl_tree *tree)
{
    return avl_height_r(tree->root);
}

int
avl_size(avl_tree *tree)
{
    return tree->size;
}

int 
avl_validate(avl_tree *tree, avl_node *node, void *ctx)
{ 
    int valid = 1;
    avl_node *left, *right;

    if (node == NULL) return valid;
    
    left = node->child[0];
    right = node->child[1];

    if (left && valid) valid = (tree->comp(AVL_DATA(left, tree), AVL_DATA(node, tree), ctx) < 0);   
    if (right && valid) valid = (tree->comp(AVL_DATA(right, tree), AVL_DATA(node, tree), ctx) > 0);

    if (valid) valid = avl_validate(tree, left, ctx);
    if (valid) valid = avl_validate(tree, right, ctx);

    return valid;
}
