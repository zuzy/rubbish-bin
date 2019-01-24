#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int findKthLargest(int* nums, int numsSize, int k) 
{
    int *tmp = nums;
    int ret, i, j;
    for(i = 0; i < k; i++) {
        ret = i;
        for(j = i + 1; j < numsSize; j++) {
            if(tmp[j] > tmp[ret])
                ret = j;
        }
        if(ret != i) {
            int t = tmp[i];
            tmp[i] = tmp[ret];
            tmp[ret] = t;
        }
    }
    return tmp[i - 1];
}

int main(int argc, char *argv[])
{
    int num[] = {3,2,1,5,6,4};
    int ret = findKthLargest(num, sizeof(num) / sizeof(int), 2);
    printf("get ret %d\n", ret);
    return 0;
}