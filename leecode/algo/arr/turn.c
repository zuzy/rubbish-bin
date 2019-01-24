#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INT_SIZE	(sizeof(int))
static int* _switch_(int *des, int *res, int size, int *tmp)
{
    if(des >= res) return NULL;
    int len = res - des;
    // int *tmp = (int *)malloc(size * INT_SIZE);
    memcpy(tmp, res, size * INT_SIZE);
    if (len >= size) {
        memcpy(res, des, size * INT_SIZE);
        memcpy(des, tmp, size * INT_SIZE);
        return des + size;
    } else {
        memcpy(des + size , des, len * INT_SIZE);
        memcpy(des, tmp, size * INT_SIZE);
        return NULL;
    }
}

void rotate(int* nums, int numsSize, int k) {
    printf(" %d %d\n", numsSize, k);
    k = k < numsSize ? k : k % numsSize;
    if(k == 0) {
        return;
    }
    int *tmp = (int *)malloc(k * INT_SIZE);
    int *ptr = nums;
    int *end = nums + numsSize - k;
    while(ptr != NULL && ptr < end) {
        ptr = _switch_(ptr, end, k, tmp);
    }
    free(tmp);
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
    rotate(nums, NUM, 2);
    for (i =0; i < NUM; i++) {
        printf("%d -> %d\n", i, nums[i]);
    }
    return 0;
}