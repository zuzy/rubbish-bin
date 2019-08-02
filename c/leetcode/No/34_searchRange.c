#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* searchRange(int* nums, int numsSize, int target, int* returnSize) {
    int s = 0, e = numsSize;
    int mid;
    int m_tmp;
    int find = 0;
    *returnSize = 2;
    int *ret = malloc(sizeof(int) * 2);
    if (numsSize <= 0 || target > nums[numsSize - 1] || target < nums[0]) goto zero;
    
    do{
        mid = (s + e) >> 1;
        if(m_tmp == mid) break;
        if(nums[mid] == target){
            find = 1;
            break;
        }
        if(nums[mid] < target) {
            s = mid;
        } else {
            e = mid;
        }
        printf("(%d %d %d)\n", s, mid, e);
        m_tmp = mid;
    }while(mid < numsSize && mid > 0);
zero:
    if(find == 0) {
        ret[0] = ret[1] = -1;
    } else {
        if(mid == 0) {
            int *p = ret;
            *p++ = 0;
            while(nums[++mid] == target);
            *p = --mid;
        } else if(mid == numsSize) {
            int *p = ret + 1;
            *p-- = mid;
            while(nums[--mid] == target);
            *p = ++mid;
        } else {
            while(mid >= 0 && nums[mid] == target){
                --mid;
            }
            ++mid;
            int *p = ret;
            *p++ = mid;
            while(nums[++mid] == target);
            *p = --mid;
        }
    }
    return ret;
}

/*
[5,7,7,8,8,10]
8

[0,0,1,1,1,4,5,5]
2
*/

int main(int argc, char *argv[])
{
    // int s[] = {5,7,8,8,8,10};
    int s[] = {0,0,1,1,1,4,5,5};
    int target = 3;
    int ret;
    int *a = searchRange(s, sizeof(s) / sizeof(int), target, &ret);
    printf("ret %d nums \n",ret);
    int i = 0;
    for(i = 0; i < ret; i++) {
        printf("a[%d] is %d\n", i, a[i]);
    }
    return 0;
}