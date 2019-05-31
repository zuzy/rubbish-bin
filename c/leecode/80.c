#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int removeDuplicates(int* nums, int numsSize) {
    if(numsSize == 0) {
        return 0;
    }
    int *ptr, i, en;
    ptr = nums + 1;
    for(i = 1, en = 0; i < numsSize; ++i) {
        if(nums[i] == nums[i - 1]) {
            ++en;
        } else {
            en = 0;
        }
        if(en <= 1) {
            *ptr++ = nums[i];
        }
    }
    return ptr-nums;
}

int main(int argc, char *argv[])
{
    char *ss = "(";
    int ret = isValid(ss);
    printf("ret is %d\n", ret);
    return 0;
}