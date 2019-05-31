#include <stdlib.h>
#include <stdio.h>

int comp(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;
}

void nextPermutation(int* nums, int numsSize) {
    printf("size %d\n", numsSize);
    if(numsSize <= 1) return;
    int i = numsSize - 1, find = 0;
    for(; i > 0; --i) {
        if(nums[i-1] < nums[i]) {
            find = 1;
            break;
        }
    }
    if(find) {
        printf("find! %d\n", i);
        int j = i;
        int tt = j;
        for(j += 1; j < numsSize; j++) {
            tt = nums[tt] < nums[j] ? tt: j;
            if(nums[j] < nums[i - 1]) {
                tt = j - 1;
                break;
            }
        }
        printf("j %d %d\n", j, nums[j]);
        int tmp = nums[tt];
        nums[tt] = nums[i - 1];
        nums[i - 1] = tmp;
        qsort(&nums[i], numsSize - i, sizeof(int), comp);
    } else {
        qsort(&nums[i], numsSize, sizeof(int), comp);
    }
}

int main(int argc, char *argv[])
{
    int *src = malloc(4 * sizeof(int));
    int i,j;
    for(i = 5, j = 0; i > 0; i--,j++) {
        src[j] = i;
    }
    // src[0] = 2;
    // src[1] = 3;
    // src[2] = 1;

    for(i = 0; i < 5; i++) {
        printf("(%d -> %d)\n", i, src[i]);
    }
    printf("-----------\n");
    nextPermutation(src, 5);
    for(i = 0; i < 5; i++) {
        printf("(%d -> %d)\n", i, src[i]);
    }
        printf("-----------\n");
    nextPermutation(src, 5);
    for(i = 0; i < 5; i++) {
        printf("(%d -> %d)\n", i, src[i]);
    }
    printf("-----------\n");
    nextPermutation(src, 5);
    for(i = 0; i < 5; i++) {
        printf("(%d -> %d)\n", i, src[i]);
    }
    printf("-----------\n");
    nextPermutation(src, 5);
    for(i = 0; i < 5; i++) {
        printf("(%d -> %d)\n", i, src[i]);
    }


    return 0;
}