#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INT_SIZE	(sizeof(int))

void moveZeroes(int* nums, int numsSize) {
    int i, *p, *re;
    
    for(i = 0, re = p = nums; i < numsSize; ++i, ++p) {
        if(*p == 0) continue;
        *re++ = *p;
    }
    memset(re, 0, (nums + numsSize - re) * INT_SIZE);
}

#define NUM 10
int main(int argc, char *argv[])
{
    int *nums = malloc(NUM * sizeof(int));
    int i = 0;
    for (i = 0; i < NUM; i++) {
        nums[i] = rand() % 10;
    }
    for (i =0; i < NUM; i++) {
        printf("%d -> %d\n", i, nums[i]);
    }
    printf("------\n");

    for (i =0; i < NUM; i++) {
        printf("%d -> %d\n", i, nums[i]);
    }
    return 0;
}