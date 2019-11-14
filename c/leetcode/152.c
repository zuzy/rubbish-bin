#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * 给定一个整数数组 nums ，
 * 找出一个序列中乘积最大的连续子序列（该序列至少包含一个数）。
 * https://leetcode-cn.com/problems/maximum-product-subarray/
 * 奇偶性
 * */

static void _dump_(int *nums, int numsSize)
{
    int i = 0;
    for(; i < numsSize; ++i) {
        printf("\t\t%d : %d\n", i, nums[i]);
    }
}

#define _dump_(nums, numsSize) NULL
#define printf(...) NULL

static int multiply(int *nums, int numsSize)
{
    int i, ret = 1;

    printf("%s : %d\n", __func__, numsSize);
    _dump_(nums, numsSize);
    for(i = 0; i < numsSize; ++i) {
        ret *= nums[i];
        printf("\t%d: %d -> %d\n", i, nums[i], ret);
    }
    return ret;
}

static int max_of_zero(int *nums, 
        int numsSize, 
        int is_odd, 
        int first_pos, 
        int last_pos)
{
    printf("size: %d, is odd: %d, first: %d, last: %d\n", numsSize, is_odd, first_pos, last_pos);
    if(numsSize == 1) {
        return nums[0];
    }
    if(is_odd) {
        // need to optimize here
        int ret = multiply(nums + first_pos + 1, numsSize - first_pos - 1);
        int ret2 = multiply(nums, last_pos);
        return ret > ret2 ? ret : ret2;

    } else {
        return multiply(nums, numsSize);
    }
    return 0;
}

int maxProduct(int* nums, int numsSize)
{
    if(numsSize == 1) {
        return nums[0];
    }
    int start, i, negative, max = 0, first_neg, last_neg;
    for(i = 0, start = 0; i < numsSize; ++i) {
        negative = 0;
        while(i < numsSize && nums[i]) {
            if(nums[i] < 0) {
                if(!negative) {
                    first_neg = i;
                }
                ++negative;
                last_neg = i;
            }
            ++i;
        }
        if(start != i) {
            printf("start: %d, i: %d\n", start, i);
            int m = max_of_zero(nums + start, 
                            i - start, 
                            negative % 2, 
                            first_neg - start,
                            last_neg - start);
            printf("m : %d\n", m);
            max = max > m ? max: m;
        }
        start = i + 1;
    }

    return max;
}

// #define END -1000

// int num[] = {2,3,-2,4, END};
// int num1[] = {1,2,3,4,5,6,7,8,9,0, END};
// int num2[] = {0,0,0,0,0, END};
// int num3[] = {2,0,3,4,0,5,6,0,7,8,0,9, END};
// int num4[] = {-1,-2,-3,-4, END};


// int **num_list = {&num[0], &num1[0], num2, num3, num4, NULL};


int main(int argc, char *argv[])
{
    // int num[] = {2,3,-2,4};
    // int num[] = {2,0,3,4,0,5,6,0,7,8,0,9};
    // int num[] = {-1,-2,-3,-4, -5};
    int num[] = {-2,0,-1};
    int ret = maxProduct(num, sizeof(num) / sizeof(num[0]));
    printf("ret is %d\n", ret);
    return 0;
}
