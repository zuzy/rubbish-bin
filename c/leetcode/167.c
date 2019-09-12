#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * https://leetcode-cn.com/problems/two-sum-ii-input-array-is-sorted/
 * 167. 两数之和 II - 输入有序数组
 * */


static void _dump_(int* numbers, int numbersSize)
{
    int i;
    printf("%s:\n", __func__);
    for(i = 0; i < numbersSize; i++) {
        printf("\t%d, %d\n", i, numbers[i]);
    }
}


/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* twoSum(int* numbers, int numbersSize, int target, int* returnSize)
{
    _dump_(numbers, numbersSize);
    int start = 0, end = numbersSize - 1;
    
    while(start < end) {
        int sum = numbers[start] + numbers[end];
        if(sum < target) {
            ++start;
        } else if(sum > target) {
            --end;
        } else {
            *returnSize = 2;
            int *ret = malloc(sizeof(int) * 2);
            ret[0] = start + 1;
            ret[1] = end + 1;
            return ret;
        }
    }
    return NULL;
}


