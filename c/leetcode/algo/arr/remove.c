#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int removeDuplicates(int* nums, int numsSize) {
    int *ptr = nums + 1;
    int i = 0;
    int step = 0;
    for (; i < numsSize; ++i) {
        if(nums[i] == nums[i - 1]) {
            ++step;
            continue;
        }
        *ptr++ = nums[i];
    }
    return numsSize - step;
}

int main(int argc, char *argv[])
{
    return 0;
}