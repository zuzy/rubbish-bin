#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

 /**
 * 119. 杨辉三角 II
 * https://leetcode-cn.com/problems/pascals-triangle-ii/submissions/
 */
#if 0
/**
 * Note: The returned array must be malloced, assume caller calls free().
 */

#define printf(...) NULL
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
    for(; i < *returnSize; i++) {
        ret[i] = get(rowIndex, i);
        printf("[%d] -> %d\n", i, ret[i]);
    }
    return ret;
}

#else

// #define printf(...) NULL
int* getRow(int rowIndex, int* returnSize){
    int i = 0;
    *returnSize = rowIndex + 1;
    int *ret = malloc(sizeof(int) * (*returnSize));
    int array[*returnSize];

    int count = *returnSize;
    for(i = 0; i < count; i++){
        array[i] = 1;
    }
    
    int size = *returnSize >> 1;

    for(i = 0; i <= size; i++) {
        ret[i] = array[--count];
        printf("%d -> %d\n", i, ret[i]);
        int j, sum = 0;
        for(j = 0; j < count; j++){
            sum += array[j];
            array[j] = sum;
        }
    }

    int *end = ret + rowIndex;
    int *start = ret;

    while(start < end) {
        *end-- = *start++;
    }
    return ret;
}
#endif

int main(int argc, char *argv[])
{
    int s = 0;
    int *ret = getRow(28, &s);
    int i = 0;
    for(; i < s; i++) {
        printf("ret [%d] -> %d\n", i, ret[i]);
    }
    return 0;
}