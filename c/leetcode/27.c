#include <stdio.h>
#include <stdlib.h>

/**
 * https://leetcode-cn.com/problems/remove-element/
 * 27. 移除元素
 * */

int removeElement(int* nums, int numsSize, int val)
{
    int i, k = 0;
    for(i = 0; i < numsSize; i++) {
        if(nums[i] != val) {
            nums[k++] = nums[i];
        }
    }
    return k;
}


