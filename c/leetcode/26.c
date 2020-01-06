#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if 1
int removeDuplicates(int* nums, int numsSize)
{
    if(numsSize <= 1) {
        return numsSize;
    }
    int *tag, *slow, *fast, i, ret;
    slow = nums;
    tag = fast = slow + 1;
    ret = 0;
    
    for(i = 1; i < numsSize; ++i, ++fast) {
        if(*slow != *fast) {
            *tag++ = *fast;
            slow = fast;
        }
    }
    return tag - nums;
}
#else
int removeDuplicates(int* nums, int numsSize) 
{
    if(numsSize <= 1) {
        return numsSize;
    }

    int *ptr = nums + 1;
    int i = 1;

    for (; i < numsSize; ++i) {
        if(nums[i] != nums[i - 1]) {
            *ptr++ = nums[i];
        }
    }

    return ptr - nums;
}
#endif

int main(int argc, char *argv[])
{
    // int a[] = {1,1,2,2};
    int a[] = {0,0,1,1,1,2,2,3,3,4,5,6,7,8,9,10,11,12,12,13,13,13,14,14,14,15,15,16,17,17,17,17,18,18,19,19,20,20,21,22,23,24,25,26,27};
    int *target = (int *)malloc(sizeof(a));
    memcpy(target, a, sizeof(a));
    int ret = removeDuplicates(target, sizeof(a)/ sizeof(a[0]));
    printf("len: %d\n", ret);
    int i = 0;
    for(; i < ret; ++i) {
        printf("\t%d", target[i]);
    }
    printf("\n");

    return 0;
}