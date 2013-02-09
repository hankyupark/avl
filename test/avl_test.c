#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "avl.h"
#include "../src/avl_private.h"

#define NNN 60000
#define MMM 60000

typedef struct intr {
    avl_node avl;
    int      data;
} intr;

typedef struct multi {
    avl_node avl[2];
    int      key[2];
} multi;


multi nmulti[NNN];
multi mmulti[MMM];

intr nintr[NNN];
intr mintr[MMM];

int  ndata[NNN];
int  mdata[MMM];

int int_compare(void *a, void *b, void *ctx) 
{
    return *((int*)a) - *((int*)b);
}


int int_print(void *n, void *ctx)
{
    printf("[%u]\n", *((int*)n));
    return 1;
}


int intr_compare(void *a, void *b, void *ctx)
{
    return ((intr*)a)->data - ((intr*)b)->data;
}


int multi_comp_0(void *a, void *b, void *c)
{
    return ((multi*)a)->key[0] - ((multi*)b)->key[0];
}

int multi_comp_1(void *a, void *b, void *c)
{
    return ((multi*)a)->key[1] - ((multi*)b)->key[1];
}


int multi_print_0(void *n, void *c)
{
    printf("[%d]\n", ((multi*)n)->key[0]);
}

int multi_print_1(void *n, void *c)
{
    printf("[%d]\n", ((multi*)n)->key[1]);
}


void
avl_dump(avl_tree *tree, avl_node *node, int level)
{
    char space[1024];
    memset(space, ' ', 1024);
    space[level*4] = 0;  

    if (node == NULL) return;

    avl_dump(tree, node->child[1], level+1);

    printf("%s[%d]\n", space, ((intr*)AVL_DATA(node, tree))->data);

    avl_dump(tree, node->child[0], level+1);
}


int main(int argc, char *argv[]) 
{
    avl_tree *ptree;
    avl_tree *itree;
    avl_tree *mtree;

    avl_node *lookup = (avl_node*)0x1;
    struct timeval start, finish;
    int i, x;



    printf("\nP-TREE:\n");

    for (i = 0; i < NNN; i++) ndata[i] = i;
    for (i = 0; i < MMM; i++) mdata[i] = i;
   
    ptree = avl_init(int_compare, NULL, 0);

    gettimeofday(&start, NULL);
    for (i = 0; i < MMM; i++) avl_insert(ptree, &mdata[i], NULL);
    //for (i = NNN; i > 0; i--) avl_insert(ptree, &ndata[i], NULL); 
    gettimeofday(&finish, NULL);
    printf("INSERT: n = %7d h = %2d v = %d (%d sec %u msec)\n", avl_size(ptree), 
                                                                avl_height(ptree), 
                                                                avl_validate(ptree, ptree->root, NULL),
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);

    gettimeofday(&start, NULL);
    for (i = 0; i < MMM && lookup; i++) lookup = avl_lookup(ptree, &mdata[i], NULL);
    gettimeofday(&finish, NULL);
    printf("LOOKUP: n = %7d h = %2d v = %d (%d sec %u msec)\n", i, 
                                                                avl_height(ptree), 
                                                                avl_validate(ptree, ptree->root, NULL),
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);

    gettimeofday(&start, NULL);

    for (i = NNN - 1; i >= 0; i--) { 
        avl_remove(ptree, &ndata[i], NULL);
    }

#if 0
    for (i = 0, srand(time(NULL)); i < NNN;) {
        x = rand() % NNN;
        if (avl_lookup(ptree, &ndata[x], NULL)) {
            avl_remove(ptree, &ndata[x], NULL);
            assert(avl_validate(ptree, ptree->root, NULL));
            i++;
        }
    }
#endif

    gettimeofday(&finish, NULL);
    printf("REMOVE: n = %7d h = %2d v = %d (%d sec %u msec)\n", avl_size(ptree),
                                                                avl_height(ptree),
                                                                avl_validate(ptree, ptree->root, NULL),
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);

    gettimeofday(&start, NULL);
    avl_free(ptree);
    gettimeofday(&finish, NULL);
    printf("DSTROY: n = %7d h = %2d v = %d (%d sec  %u msec)\n", 0, 
                                                                 0,
                                                                 1,
                                                                 (int)         (finish.tv_sec  - start.tv_sec ),
                                                                 (unsigned int)(finish.tv_usec - start.tv_usec)/1000); 
   

    printf("\nI-TREE:\n");

    memset(nintr, 0, NNN * sizeof(intr));
    memset(mintr, 0, MMM * sizeof(intr));
    for (i = 0; i < NNN; i++) nintr[i].data = i;
    for (i = 0; i < MMM; i++) mintr[i].data = i;

    itree = avl_init(intr_compare, NULL, AVL_TREE_INTRUSIVE);


    gettimeofday(&start, NULL);
    for (i = 0; i < MMM; i++) avl_insert(itree, &mintr[i], NULL);
    //for (i = NNN - 1; i >= 0; i--) avl_insert(itree, &nintr[i], NULL); 
    gettimeofday(&finish, NULL);
    printf("INSERT: n = %7d h = %2d v = %d (%d sec %u msec)\n", avl_size(itree), 
                                                                avl_height(itree),
                                                                avl_validate(itree, itree->root, NULL),
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);

    gettimeofday(&start, NULL);
    for (i = 0; i < MMM && lookup; i++) lookup = avl_lookup(itree, &mintr[i], NULL);
    gettimeofday(&finish, NULL);
    printf("LOOKUP: n = %7d h = %2d v = %d (%d sec %u msec)\n", i, 
                                                                avl_height(itree), 
                                                                avl_validate(itree, itree->root, NULL),
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);

    gettimeofday(&start, NULL);

   
    for (i = NNN - 1; i >= 0; i--) { 
        avl_remove(itree, &nintr[i], NULL);
    }

#if 0
    for (i = 0, srand(time(NULL)); i < NNN;) {
        x = rand() % NNN;
        if (avl_lookup(itree, &nintr[x], NULL)) {
            avl_remove(itree, &nintr[x], NULL);
            assert(avl_validate(itree, itree->root, NULL));
            i++;
        }
    }
#endif

    gettimeofday(&finish, NULL);
    printf("REMOVE: n = %7d h = %2d v = %d (%d sec %u msec)\n", avl_size(itree),
                                                                avl_height(itree),
                                                                avl_validate(itree, itree->root, NULL),
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);


    gettimeofday(&start, NULL);
    avl_free(itree);
    gettimeofday(&finish, NULL);
    printf("DSTROY: n = %7d h = %2d v = %d (%d sec  %u msec)\n\n", 0, 
                                                                   0,
                                                                   1,
                                                                   (int)         (finish.tv_sec  - start.tv_sec ),
                                                                   (unsigned int)(finish.tv_usec - start.tv_usec)/1000); 




    printf("\nM-TREE:\n");

    memset(nmulti, 0, NNN * sizeof(multi));
    memset(mmulti, 0, MMM * sizeof(multi));
    for (i = 0; i < NNN; i++) {nmulti[i].key[0] = i;nmulti[i].key[1] = i+1;}
    for (i = 0; i < MMM; i++) {mmulti[i].key[0] = i;mmulti[i].key[1] = i+1;}


    avl_compare_fn comps[2] = { multi_comp_0, multi_comp_1 };

    mtree = avl_multi_init(comps, NULL, 2, AVL_TREE_INTRUSIVE);


    gettimeofday(&start, NULL);
    for (i = 0; i < MMM; i++) avl_multi_insert(mtree, &mmulti[i], NULL);
    //for (i = NNN - 1; i >= 0; i--) avl_multi_insert(mtree, &nmulti[i], NULL); 
    gettimeofday(&finish, NULL);
    printf("INSERT: n = %7d h = %2d v = %d (%d sec %u msec)\n", avl_size(mtree), 
                                                                avl_height(mtree),
                                                                avl_validate(mtree, mtree->root, NULL) &&
                                                                avl_validate(&mtree[0], mtree[0].root, NULL) ==
                                                                avl_validate(&mtree[1], mtree[1].root, NULL) ,
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);

    gettimeofday(&start, NULL);
    for (i = 0; i < MMM && lookup; i++) lookup = avl_lookup(mtree, &mmulti[i], NULL);
    gettimeofday(&finish, NULL);
    printf("LOOKUP: n = %7d h = %2d v = %d (%d sec %u msec)\n", avl_size(mtree), 
                                                                avl_height(mtree),
                                                                avl_validate(mtree, mtree->root, NULL) &&
                                                                avl_validate(&mtree[0], mtree[0].root, NULL) ==
                                                                avl_validate(&mtree[1], mtree[1].root, NULL) ,
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);

    gettimeofday(&start, NULL);


    //avl_walk(&mtree[0], multi_print_0, NULL, 0); printf("---\n");fflush(stdout);
    //avl_walk(&mtree[1], multi_print_1, NULL, 0); printf("===\n");fflush(stdout);

   
    for (i = MMM - 1; i >= 1000; i--) { 
        avl_multi_remove(mtree, &mmulti[i], NULL);
    }

    //avl_walk(&mtree[0], multi_print_0, NULL, 0); printf("---\n");fflush(stdout);
    //avl_walk(&mtree[1], multi_print_1, NULL, 0); printf("===\n");fflush(stdout);

#if 0
    for (i = 0, srand(time(NULL)); i < NNN;) {
        x = rand() % NNN;
        if (avl_lookup(itree, &nintr[x], NULL)) {
            avl_remove(itree, &nintr[x], NULL);
            assert(avl_validate(itree, itree->root, NULL));
            i++;
        }
    }
#endif

    gettimeofday(&finish, NULL);
    printf("REMOVE: n = %7d h = %2d v = %d (%d sec %u msec)\n", avl_size(mtree), 
                                                                avl_height(mtree),
                                                                avl_validate(mtree, mtree->root, NULL) &&
                                                                avl_validate(&mtree[0], mtree[0].root, NULL) ==
                                                                avl_validate(&mtree[1], mtree[1].root, NULL) ,
                                                                (int)         (finish.tv_sec  - start.tv_sec ), 
                                                                (unsigned int)(finish.tv_usec - start.tv_usec)/1000);


    gettimeofday(&start, NULL);
    avl_free(mtree);
    gettimeofday(&finish, NULL);
    printf("DSTROY: n = %7d h = %2d v = %d (%d sec  %u msec)\n\n", 0, 
                                                                   0,
                                                                   1,
                                                                   (int)         (finish.tv_sec  - start.tv_sec ),
                                                                   (unsigned int)(finish.tv_usec - start.tv_usec)/1000); 

  

    return 0;
}

