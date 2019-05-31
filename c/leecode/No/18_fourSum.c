#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Return an array of arrays of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
static int _cmp(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;
}

static int _check_(int **des, int size, int a, int b, int c, int d)
{
    if(!size) return 1;
    int i = 0;
    for(; i < size; i++) {
        if(des[i][0] == a && 
        des[i][1] == b &&
        des[i][2] == c) {
            printf("(%d %d %d %d) jmp\n", a, b, c, d);
            return 0;
        }
    }
    printf("(%d %d %d %d) final\n", a, b, c, d);
    return 1;
}

int** fourSum(int* nums, int numsSize, int target, int* returnSize) {
    if(numsSize < 4) return NULL;
    qsort(nums, numsSize, sizeof(int), _cmp);
    *returnSize = 0;
    // int tmp = numsSize;
    int msize = 1000;
    // for(;tmp > 4; tmp--) {
    //     msize *= tmp;
    // }
    int **ret = malloc(msize * sizeof(int *));

    int **p = ret;
    int i, j, k, l;
    int s1 = numsSize - 1, s2 = numsSize - 2, s3 = numsSize - 3;
    int t1, t2, t3;
    for(i = 0; i < s3; i++) {
        for(j = i + 1; j < s2; j++) {
            t1 = nums[i] + nums[j];
            #if 0
                for(k = j + 1; k < s1; k++) {
                    t2 = t1 + nums[k];
                    for(l = k + 1; l < numsSize; l++) {
                        t3 = t2 + nums[l];
                        if(t3 > target) break;
                        if(t3 == target) {
                            printf("get !\n");
                            if(_check_(ret, *returnSize, nums[i], nums[j], nums[k], nums[l])) {
                                printf("check ok !\n");
                                *p = malloc(4*sizeof(int));
                                (*p)[0] = nums[i];
                                (*p)[1] = nums[j];
                                (*p)[2] = nums[k];
                                (*p)[3] = nums[l];
                                ++*returnSize;
                                ++p;
                            }
                            break;
                        }
                    }
                }
            #else
            t2 = target - t1;
            printf("t2 %d\n", t2);
            for(k = j + 1, l = numsSize - 1; k < l; ) {
                printf("(%d %d) -> (%d %d)\n", k, l, nums[k], nums[l]);
                if(t2 < nums[k] + nums[l]) {
                    --l;
                } else if(t2 > nums[k] + nums[l]) {
                    ++k;
                } else {
                    printf("get!\n");
                    if(_check_(ret, *returnSize, nums[i], nums[j], nums[k], nums[l])) {
                        printf("check ok !\n");
                        *p = malloc(4*sizeof(int));
                        (*p)[0] = nums[i];
                        (*p)[1] = nums[j];
                        (*p)[2] = nums[k];
                        (*p)[3] = nums[l];
                        ++*returnSize;
                        ++p;
                    }
                    break;
                }
            }
            #endif
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    return 0;
}