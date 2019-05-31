#include <stdio.h>
#include <stdlib.h>

typedef int (*__compar_fn_t) (const void *, const void *);

int comp(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;
}

int longestConsecutive(int* nums, int numsSize) {
    if(numsSize == 0)
        return 0;
    if(numsSize == 1)
        return 1;
    
    qsort(nums, numsSize, sizeof(int), comp);

    int max = 0;
    int i = 1;
    int offset = 0;
    while(1) {
        int len = 1;
        for( i = 1; i + offset < numsSize; i++) {
            if(nums[i + offset] == nums[i - 1 + offset]) {
                continue;
            }
            if(1 != nums[i + offset] - nums[i - 1 + offset]) {
                break;
            }
            ++len;
        } 
        if(len > max) {
            max = len;
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
    int nums[] = {1,3,5,4,2,3,4,5};
    int ret = findLengthOfLCIS(nums, sizeof(nums) / sizeof(int));
    printf("get %d\n", ret);
}