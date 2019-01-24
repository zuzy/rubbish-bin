#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INT_MAX 2147483647

int singleNumber(int* nums, int numsSize) {
    int i, j;
    for (i = 0; i < numsSize; i++)
    {
        if(nums[i] == INT_MAX) {
            continue;
        }
        for (j = i + 1; j < numsSize; j++) {
            if(nums[i]== nums[j]) {
                nums[i] = nums[j] = INT_MAX;
                break;
            }
        }
    }
    for (i = 0; i < numsSize; i++) {
        if(nums[i] != INT_MAX) {
            return nums[i];
        }
    }
}

int main(int argc, char *argv[])
{
    int nums[] = {4,1,2,1,2};
    int ret = singleNumber(nums, sizeof(nums) / sizeof(nums[0]));
    printf("get ret %d \n", ret);
    return 0;
}