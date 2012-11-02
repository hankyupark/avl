#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "avl.h"
#include "../src/avl_private.h"

#define NNN 10000
#define MMM 50000

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
    avl_tree *intr_tree;
    avl_node *lookup = (avl_node*)0x1;
    struct timeval start, finish;
    int i;


    for (i = 0; i < NNN; i++) ndata[i] = i;
    for (i = 0; i < MMM; i++) mdata[i] = i;
   
    ptree = avl_new(int_compare, NULL, 0);

    gettimeofday(&start, NULL);
    for (i = 0; i < MMM; i++) avl_insert(ptree, &mdata[i], NULL);
    for (i = NNN; i > 0; i--) avl_insert(ptree, &ndata[i], NULL); 
    gettimeofday(&finish, NULL);
    printf("PLAIN INSERTION: %d seconds  %u microseconds\n", (int)         (finish.tv_sec - start.tv_sec), 
                                                             (unsigned int)(finish.tv_usec - start.tv_usec));
    printf("PLAIN INSERTION: %d nodes\n", avl_size(ptree));
    printf("PLAIN INSERTION: %d height\n", avl_height(ptree));


    gettimeofday(&start, NULL);
    for (i = 0; i < MMM && lookup; i++) lookup = avl_lookup(ptree, &mdata[i], NULL);
    gettimeofday(&finish, NULL);
    printf("   PLAIN LOOKUP: last index: %d\n", i);
    printf("   PLAIN LOOKUP: %d seconds  %u microseconds\n", (int)         (finish.tv_sec - start.tv_sec), 
                                                             (unsigned int)(finish.tv_usec - start.tv_usec));

     
    gettimeofday(&start, NULL);
    for (i = 0; i < NNN; i++) avl_remove(ptree, &ndata[i], NULL);
    gettimeofday(&finish, NULL);
    printf("  PLAIN REMOVAL: %d seconds  %u microseconds\n", (int)         (finish.tv_sec - start.tv_sec), 
                                                             (unsigned int)(finish.tv_usec - start.tv_usec));
    printf("  PLAIN REMOVAL: %d nodes left\n", avl_size(ptree));
    

    gettimeofday(&start, NULL);
    avl_free(ptree);
    gettimeofday(&finish, NULL);
    printf("     PLAIN FREE: %d seconds  %u microseconds\n", (int)         (finish.tv_sec  - start.tv_sec),
                                                             (unsigned int)(finish.tv_usec - start.tv_usec)); 


    memset(nintr, 0, NNN * sizeof(intr));
    memset(mintr, 0, MMM * sizeof(intr));
    for (i = 0; i < NNN; i++) nintr[i].data = i;
    for (i = 0; i < MMM; i++) mintr[i].data = i;
   
    intr_tree = avl_new(intr_compare, NULL, AVL_TREE_INTRUSIVE);

    gettimeofday(&start, NULL);
    for (i = 0; i < MMM; i++)      avl_insert(intr_tree, &mintr[i], NULL);
    for (i = NNN - 1; i >= 0; i--) avl_insert(intr_tree, &nintr[i], NULL); 
    gettimeofday(&finish, NULL);
    printf(" INTR INSERTION: %d seconds  %u microseconds\n", (int)         (finish.tv_sec  - start.tv_sec), 
                                                             (unsigned int)(finish.tv_usec - start.tv_usec));
    printf(" INTR INSERTION: %d nodes\n", avl_size(intr_tree));


    gettimeofday(&start, NULL);
    for (i = 0; i < MMM && lookup; i++) lookup = avl_lookup(intr_tree, &mintr[i], NULL);
    gettimeofday(&finish, NULL);
    printf("    INTR LOOKUP: last index: %d\n", i);
    printf("    INTR LOOKUP: %d seconds  %u microseconds\n", (int)         (finish.tv_sec  - start.tv_sec), 
                                                             (unsigned int)(finish.tv_usec - start.tv_usec));

     
    gettimeofday(&start, NULL);
    for (i = 0; i < NNN; i++) avl_remove(intr_tree, &nintr[i], NULL);
    gettimeofday(&finish, NULL);
    printf("   INTR REMOVAL: %d seconds  %u microseconds\n", (int)         (finish.tv_sec  - start.tv_sec), 
                                                             (unsigned int)(finish.tv_usec - start.tv_usec));
    printf("   INTR REMOVAL: %d nodes left\n", avl_size(intr_tree));
    

    gettimeofday(&start, NULL);
    avl_free(intr_tree);
    gettimeofday(&finish, NULL);
    printf("      INTR FREE: %d seconds  %u microseconds\n", (int)         (finish.tv_sec  - start.tv_sec),
                                                             (unsigned int)(finish.tv_usec - start.tv_usec)); 

    

    return 0;
}

