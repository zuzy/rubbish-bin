#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */


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

    for(i = 0; i < size; i++) {
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