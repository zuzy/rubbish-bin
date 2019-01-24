#include <stdlib.h>
#include <stdio.h>

/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *columnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */

int _combine_(int *candidates, int candidatesSize, int target, int **ret, int size, int *index, int *calsize)
{
    int i = 0;
    for(; i < candidatesSize; i++) {
        if(candidates[i] == target) {
            int **p = ret;
            while(**p != 0) ++p;
            *index = p - ret;
            **p = candidates[i];
            int *t = calsize;
            while(*t) ++t;
            *t = size;
            return 1;
        } else if(candidates[i] < target) {
            if(_combine_(candidates, candidatesSize, target - candidates[i], ret, size + 1, index, calsize)) {
                
            }
        } else {
            return 0;
        }
    }
    return 0;
}

int** combinationSum(int* candidates, int candidatesSize, int target, int** columnSizes, int* returnSize) {
    int **ret = calloc(1000, sizeof(int *));
    *columnSizes = calloc(1000, sizeof(int));
    _combine_(candidates, candidatesSize, target, )
}

int main(int argc, char *argv[])
{
    return 0;
}