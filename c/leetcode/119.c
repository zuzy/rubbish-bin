#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */


static int get(int row, int col)
{
    if(row < 0 || col < 0 || row < col) {
        return 0;
    }
    printf("(%d %d)", row, col);
    if(col == 0) {
        printf(" --> 1\n");
        return 1;
    } 
    if(col == row) {
        printf(" --> 1\n");
        return 1;
    }
    printf("\n");
    return get(row - 1, col - 1) + get(row - 1, col);
}

int* getRow(int rowIndex, int* returnSize){
    int i = 0;
    *returnSize = rowIndex + 1;
    int *ret = malloc(sizeof(int) * (*returnSize));
    printf("return size %d\n", *returnSize);
    int size = *returnSize / 2;
    for(; i < size; i++) {
        ret[i] = get(rowIndex, i);
        printf("[%d] -> %d\n", i, ret[i]);
    }
    size = *returnSize - 1;
    for(; i < *returnSize; i++) {
        ret[i] = ret[size - i];
    }
    return ret;
}


int main(int argc, char *argv[])
{
    int s = 0;
    int *ret = getRow(3, &s);
    int i = 0;
    for(; i < s; i++) {
        printf("ret [%d] -> %d\n", i, ret[i]);
    }
    return 0;
}