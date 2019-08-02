#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/**
 * Note: The returned array must be malloced, assume caller calls free().
 */

#define INT_SIZE	(sizeof(int))
int* twoSum(int* nums, int numsSize, int target) {
    int i, j;
    for(i = 0; i < numsSize; ++i) {
        for(j = i + 1; j < numsSize; ++j) {
            printf("(%d, %d) --> %d, %d\n", i, j, nums[i], nums[j]);
            if((nums[i] + nums[j]) == target) {
                printf("ok %d %d\n", i, j);
                int *ret = (int *)malloc(2 * INT_SIZE);
                ret[0] = i;
                ret[1] = j;
                return ret;
            }
        }
    }
    return NULL;
}

#define NUM 10
int main(int argc, char *argv[])
{
    int i = 0;
    #if 0
    int *nums = malloc(NUM * sizeof(int));
    for (i = 0; i < NUM; i++) {
        nums[i] = rand() % 10;
    }
    for (i =0; i < NUM; i++) {
        printf("%d -> %d\n", i, nums[i]);
    }
    printf("------\n");
    int *ret = twoSum(nums, NUM ,6);
    #else
    int nums[] = {3, 2, 4};
    int n =  3;
    int *ret = twoSum(nums, n ,6);
    #endif
    // for (i =0; i < NUM; i++) {
    //     printf("%d -> %d\n", i, nums[i]);
    // }
    if(ret) {
        for (i =0; i < 2; i++) {
            printf("%d -> %d\n", i, ret[i]);
        }
    }
    return 0;
}