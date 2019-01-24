#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Return an array of arrays of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */

#define INT_MAX (0x80000000-1)
#define INT_MIN (-2147483648)
#define TARGET(t, a, b)  (abs(t-a) <= abs(t-b)) ? (a): (b)
int compare_ints(const void* a, const void* b)
{
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
    return (arg1 > arg2) - (arg1 < arg2); // 可行的简写
}

int threeSumClosest(int* nums, int numsSize, int target) {
    qsort(nums, numsSize, sizeof(int), compare_ints);
    int i, ret, tmp;
    ret = nums[0] + nums[1] + nums[2] - target;
    printf("ret is %d\n", ret);
    for(i = 0; i < numsSize - 2; i++) {
        int start = i + 1;
        int end = numsSize - 1;
        while (start < end) {
            int tmp = nums[i] + nums[start] + nums[end] - target;
            if(tmp == 0) {
                return target;
            } else if(tmp > 0) {
                --end;
            } else {
                ++start;
            }
            ret = abs(ret) < abs(tmp) ? ret : tmp;
        }
    }
    return ret + target;
}

int main(int argc, char *argv[])
{
    // int nums[] = {0, 1, 2};
    // int target = 0;
    // int nums[] = {1,1,-1,-1,3};
    // int target = -1;

    // int nums[]= {1,1,-1};
    // int target = 2;

    // int nums[] = {1,1,1,0};
    // int target = 100;
    // int nums[] = {-1,2,1,-4};
    // int target = 1;

    int nums[] = {0, 2, 1, -3};
    int target = 1;
    int ret;
    ret = threeSumClosest(nums, sizeof(nums)/sizeof(nums[0]), target);
    printf("get ret is %d\n", ret);

    return 0;
}