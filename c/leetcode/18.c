#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Return an array of arrays of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */

int comp(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;
}

static int check(int **num, int *size, int target) {
    int *s = *num;
    int *e = *num + *size - 1;
    while(s < e) {
        if((*s + *e) == target) {
            *size = *size -(s - *num);
            *num = s;
            return 0;
        }
        if((*s + *e) > target) {
            --e;
        } else {
            ++s;
        }
    }
    return 1;
}

static void _add(int **des, int *arr) {
    int **p = des;
    while(*p) {
        if(memcmp(*p, arr, 4 * sizeof(int)) == 0) {
            return;
        }
        ++p;
    }
    *p = arr;
    return;
}

int** fourSum(int* nums, int numsSize, int target, int* returnSize) {
    qsort(nums, numsSize, sizeof(int), comp);
    int **s = (int **)calloc(1024, sizeof(int));
    int i,j;
    for(i = 0; i < numsSize - 3; ++i) {
        if(i && nums[i] == nums[i-1]) continue;

        for(j = i; j < numsSize - 2; ++j) {
            if(j && nums[j] == nums[j - 1]) continue;
            int targ = target - nums[i] - nums[j];
            int *tmp = nums;
            int size = numsSize;
            while(check(&tmp, &size, ))
        }
    }
    return s;
}

int main(int argc, char *argv[])
{
    char *input = "";
    int ret = 0;
    char **p = letterCombinations(input, &ret);
    char **pp = p;
    printf("get %d\n", ret);
    while(*pp) {
        printf("%s\n", *pp);
        ++pp;
    }

    return 0;
}