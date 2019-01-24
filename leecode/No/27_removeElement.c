#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int removeElement(int* nums, int numsSize, int val) {
    int *p = nums + numsSize - 1;
    int ret = numsSize;
    int i;
    for(i = 0; i < ret; i++) {
        if(nums[i] == val) {
            --ret;
            if(ret <= 0) {
                return 0;
            }
            if(nums + i == p) {
                return ret;
            }
            while(*p == val) {
                --p;
                --ret;
                if(ret <= 0)
                    return 0;
                if(p - nums <= i) {
                    break;
                }
            }
            nums[i] = *p;
            --p;
        }
        // printf("%d ", i);
        // if(i >= p - nums) {
        //     break;
        // }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    // int s[] = {0,1,2,2,3,0,4,2};
    // int s[] = {4,5};
    int s[] = {2,3,3};
    int *res = (int *)malloc(sizeof(s));
    memcpy(res, s, sizeof(s));
    int i;
    for(i = 0; i < sizeof(s) / sizeof(int); i++) {
        printf("%d -> %d\n", i, res[i]);
    }
    int r = removeElement(res, sizeof(s) / sizeof(int), 3);

    for(i = 0; i < r; i++) {
        printf("%d -> %d\n", i, res[i]);
    }
    return 0;
}