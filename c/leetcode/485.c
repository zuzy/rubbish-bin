#include <stdio.h>
#include <stdlib.h>

/**
 * https://leetcode-cn.com/problems/max-consecutive-ones/
 * 485. 最大连续1的个数
 * */

int findMaxConsecutiveOnes(int* nums, int numsSize)
{
    int i, one = 0, max = 0;
    for(i = 0; i < numsSize; i++) {
        if(nums[i] == 1) {
            ++one;
        } else if(one) {
            max = max < one ? one : max;
            one = 0;
        }
    }
    if(one) {
        max = max < one ? one : max;
    }
    return max;
}

