#include <stdio.h>
#include <stdlib.h>

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */

int daily_match(int *T, int TSize) {
    int i = 1;
    for(; i < TSize; i++) {
        if(T[0] < T[i]) {
            return i;
        }
    }
    return 0;
}

int* dailyTemperatures(int* T, int TSize, int* returnSize){
    int *ret = malloc(TSize * sizeof(int));
    *returnSize = TSize;
    int *p = ret;
    int *tag = T;
    int i = 0;
    int size = TSize;
    for(; i < TSize; i++) {
        *p++ = daily_match(tag++, size--);
    }
    return ret;
}


int main(int argc, char *argv[])
{

}