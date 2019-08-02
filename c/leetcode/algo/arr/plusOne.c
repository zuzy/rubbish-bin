#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
#define INT_SIZE	(sizeof(int))
void revert(int *nums, int size) {
    int *ps, *pe;
    for(ps = nums, pe = nums + size - 1; ps < pe; ++ps, --pe) {
        int tmp = *ps;
        *ps = *pe;
        *pe = tmp;
    }
}
int* plusOne(int* digits, int digitsSize, int* returnSize) {
    *returnSize = digitsSize + 1;
    int *ret = malloc(*returnSize * INT_SIZE);
    memcpy(ret, digits, digitsSize * INT_SIZE);
    revert(ret, digitsSize);
    int *p;
    int i;
    int en = 1;
    for(i = 0, p = ret; i < digitsSize; ++i, ++p) {
        if(en) ++*p;
        // printf("\t (%d, %d)\n", i, *p);
        if(*p == 10) {
            en = 1;
            *p = 0;
        } else {
            en = 0;
        }
    }
    if(en) *p = 1;
    else {
        *p = 0;
        --*returnSize;
    }
    revert(ret, *returnSize);
    return ret;
}
#define NUM 10
int main(int argc, char *argv[])
{
    int *nums = malloc(NUM * sizeof(int));
    int i = 0;
    for (i = 0; i < NUM; i++) {
        nums[i] = rand() % 10;
    }
    for (i =0; i < NUM; i++) {
        printf("%d -> %d\n", i, nums[i]);
    }
    printf("------\n");
    int ret;
    plusOne(nums, NUM, &ret);
    for (i =0; i < NUM; i++) {
        printf("%d -> %d\n", i, nums[i]);
    }
    return 0;
}