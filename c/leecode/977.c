#include <stdio.h>
#include <stdlib.h>

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int _comp (const void *A, const void *B) 
{
    int *a = (int *)A;
    int *b = (int *)B;
    return abs(*a) - abs(*b);
}

int* sortedSquares(int* A, int ASize, int* returnSize){
    qsort(A, ASize, sizeof(int), _comp);
    int *ret = malloc(ASize * sizeof(A));
    *returnSize = ASize;
    int i;
    for(i = 0; i < ASize; i++) {
        ret[i] = A[i] * A[i];
    }
    return ret;
}


int main(int argc, char *argv[])
{
    return 0;
}