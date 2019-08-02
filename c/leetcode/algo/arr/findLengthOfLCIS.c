#include <stdio.h>
#include <stdio.h>

int findLengthOfLCIS(int* nums, int numsSize) {
    if(numsSize == 0)
        return 0;
    if(numsSize == 1)
        return 1;
    
    int max = 0;
    int i = 1;
    int offset = 0;
    while(1) {
        int diff = 0;
        for( i = 1; i + offset < numsSize; i++) {
            if(nums[i + offset] <= nums[i - 1 + offset]) 
                break;
            
        } 
        if(i > max) {
            max = i;
        }
        if(numsSize <= i + offset) {
            break;
        }
        offset += i;
        printf("offset is %d\n", offset);
    }
    return max;
}

int main(int argc, char *argv[])
{
    // int nums[] = {1,3,5,4,2,3,4,5};
    int nums[] = {2,2,2,2,2};
    int ret = findLengthOfLCIS(nums, sizeof(nums) / sizeof(int));
    printf("get %d\n", ret);
}