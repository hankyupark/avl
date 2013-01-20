#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "avl.h"
#include "../src/avl_private.h"

#define NNN 6000000
#define MMM 6000000

typedef struct intr {
    avl_node avl;
    int      data;
} intr;

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



int main(int argc, char *argv[]) 
{
    avl_tree *ptree;
    avl_tree *itree;
    avl_node *lookup = (avl_node*)0x1;
    struct timeval start, finish;
    int i;


    for (i = 0; i < NNN; i++) ndata[i] = i;
    for (i = 0; i < MMM; i++) mdata[i] = i;
   
    ptree = avl_new(int_compare, NULL, 0);



   
    printf("\nP-TREE:\n");


    gettimeofday(&start, NULL);
    for (i = 0; i < MMM; i++) avl_insert(ptree, &mdata[i], NULL);
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
    for (i = 0; i < NNN; i++) avl_remove(ptree, &ndata[i], NULL);
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


    memset(nintr, 0, NNN * sizeof(intr));
    memset(mintr, 0, MMM * sizeof(intr));
    for (i = 0; i < NNN; i++) { nintr[i].avl = AVL_INODE; nintr[i].data = i; }
    for (i = 0; i < MMM; i++) { mintr[i].avl = AVL_INODE; mintr[i].data = i; }
   

    printf("\n-----------------\n");

    printf("\nI-TREE:\n");

    itree = avl_new(intr_compare, NULL, AVL_TREE_INTRUSIVE);


    gettimeofday(&start, NULL);
    for (i = 0; i < MMM; i++) avl_insert(itree, &mintr[i], NULL);
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
    for (i = 0; i < NNN; i++) avl_remove(itree, &nintr[i], NULL);
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

  

    return 0;
}

