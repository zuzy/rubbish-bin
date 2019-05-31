#include <stdio.h>
#include <stdlib.h>

int searchInsert(int* nums, int numsSize, int target) {
    if(numsSize == 0 || target <= *nums) return 0;
    if(target > nums[numsSize - 1]) return numsSize;
    int s = 0, e = numsSize, mid;
    do {
        mid = (s + e) >> 1;
        if(nums[mid] == target) {
            return mid;
        } else if(nums[mid] > target) {
            if(nums[mid - 1] < target) return mid;
            e = mid;
        } else {
            if(nums[mid + 1] > target) return mid + 1;
            s = mid;
        }
    }while(1);
}

int main(int argc ,char *argv[])
{
    int nums[] = {1,3,5,6};
    int target = 0;
    int ret = searchInsert(nums, sizeof(nums) / sizeof(int), target);
    printf("ret is %d\n", ret);
    return 0;
}