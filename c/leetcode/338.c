#include <stdio.h>
#include <stdlib.h>

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* countBits(int num, int* returnSize){
    if(num < 0) {
        *returnSize =  0;
        return NULL;
    }
    *returnSize = ++num;
    int *ret = (int *)malloc(num * sizeof(int));
    int *p = ret;
    int i;
    int index = 0, tmp;
    *p++ = 0;
    for(i = 1; i < num; i++) {
        tmp = 1 << index;
        if(i < tmp) {
            tmp >>= 1;
            // *p++ = ret[tmp] + ret[i-tmp];
            *p++ = 1 + ret[i-tmp];
        } else {
            ++index;
            *p++ = 1;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int len = 0;
    int *ret = countBits(atoi(argv[1]), &len);
    int i = 0;
    for(; i < len; i++) {
        printf(" %d -> %d\n", i, ret[i]);
    }
    free(ret);
    return 0;
}