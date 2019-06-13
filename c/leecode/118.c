#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
int** generate(int numRows, int* returnSize, int** returnColumnSizes){
    *returnSize = numRows;
    if(numRows <= 0) {
        *returnColumnSizes = (int *)malloc(sizeof(int));
        **returnColumnSizes = 0;
        return NULL;
    }
    int **ret = (int **)malloc(numRows * sizeof(int *));
    *returnColumnSizes = (int *)malloc(numRows * sizeof(int));
    int i = 0;
    for(; i < numRows; i++) {
        int size = i + 1;
        (*returnColumnSizes)[i] = size;
        int n = 0;
        int mid = size >> 1;
        ret[i] = (int *)malloc(sizeof(int) * size);
        int *p = ret[i];
        for(; n < mid; n++) {
            if(n == 0) {
                *p++ = 1;
            } else {
                *p++ = ret[i - 1][n - 1] + ret[i - 1][n];
            }
        }
        for(; n < size; n++) {
            if(n == 0 || n == size - 1) {
                *p++ = 1;
            } else {
                *p++ = ret[i - 1][n - 1] + ret[i - 1][n];
            }
        }
    }
    return ret;
}

