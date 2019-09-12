#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

/**
 * https://leetcode-cn.com/problems/array-partition-i/
 * 561. 数组拆分 I
 * */

static int _comp_(const void *a, const void *b)
{
    int a1 = *(int *)a;
    int b1 = *(int *)b;
    return (b1 < a1) - (a1 < b1);
}

int arrayPairSum(int* nums, int numsSize){
    if(!numsSize) 
        return 0;

    qsort(nums, numsSize, sizeof(int), _comp_);
    int i, ret = 0;
    for(i = 0; i < numsSize; i+= 2) {
        ret += nums[i];
    }
    return ret;
}

