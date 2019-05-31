#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
#define INT_SIZE	(sizeof(int))
int* intersect(int* nums1, int nums1Size, int* nums2, int nums2Size, int* returnSize) {
    int *p1, *p2, p1_size, p2_size;
    if(nums1Size < nums2Size) {
        p1 = nums1;
        p1_size = nums1Size;
        p2 = nums2;
        p2_size = nums2Size;
    } else {
        p1 = nums2;
        p1_size = nums2Size;
        p2 = nums1;
        p2_size = nums1Size;
    }
    int i, j;
    int *ret = calloc(1, p1_size * INT_SIZE);
    int *p_ret = ret;
    *returnSize = 0;
    int p1_zero = 0;
    int p2_zero = 0;
    for(i = 0; i < p1_size; ++i)
        if(p1[i] == 0) ++p1_zero;
    for(j = 0; j < p2_size; ++j)
        if(p2[j] == 0) ++p2_zero;
    int len = p1_zero < p2_zero ? p1_zero: p2_zero;
    *returnSize += len;
    p_ret += len;

    for(i = 0; i < p1_size; ++i) {
        if(p1[i] == 0) continue;
        for(j = 0; j <p2_size; ++j) {
            if(p2[j] == 0) continue;
            if(p1[i] == p2[j]) {
                *p_ret++ = p1[i];
                ++*returnSize;
                p2[j] = 0;
                break;
            }
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    return 0;
}