#include <stdio.h>
#include <stdlib.h>

#define printf(...) NULL
int check(int des, int tar)
{
    if(des == tar) {
        return 0;
    } else {
        if(des > tar) {
            return 1;
        } else {
            return 2;
        }
    }
}

int search(int* nums, int numsSize, int target) {
    if(numsSize <= 0) {
        return -1;
    }
    int start = 0;
    int end = numsSize - 1;
    int mid;
    int en = 0;
    if(target == nums[0]) {
        return 0;
    }
    
    while(1) {
        mid = (end + 1 + start) / 2;
        printf("check %d -> %d\n", mid, nums[mid]);
        if(target == nums[mid]) {
            return mid;
        }
        if(target > *nums) {
            if(target < nums[mid]) {
                printf("%d\tfind %d %d %d\n", __LINE__, start, mid, end);
                break;
            }
        } else {
            if(target > nums[mid]) {
                printf("%d\tfind %d %d %d\n", __LINE__, start, mid, end);
                break;
            }
        }
        if(mid == end) {
            return -1;
        }
        if(nums[mid] < nums[0]) {
            end = mid;
        } else {
            start = mid;
        }
        printf("%d %d %d\n", start, mid, end);
        // usleep(200000);
    }
    printf("(%d, %d, %d) -> (%d, %d, %d)\n", start, mid, end, nums[start], nums[mid], nums[end]);
    End: {
        if(target > nums[mid]) 
                start = mid;
            else
                end = mid;

        if(target > *nums && nums[mid]){

        }

        printf("%d\tfind %d %d %d\n", __LINE__, start, mid, end);
        while(1) {
            mid = (start + end + 1) / 2;
            int ret = check(nums[mid], target);
            if(ret) {
                if(mid == end) {
                    return -1;
                } else {
                    if(ret == 1) {
                        end = mid;
                    } else {
                        start = mid;
                    }
                }
            } else {
                return mid;
            }
            printf("%d\tcheck %d %d %d\n", __LINE__, start, mid, end);
            // usleep(200000);
        }
    }
    return -1;
}
/*
{4,5,6,7,0,1,2}
{1,2,4,5,6,7,0}
3
[15,16,19,20,25,1,3,4,5,7,10,14]
25
*/
int main(int argc, char *argv[])
{
    int nums[] = {4,5,6,7,0,1,2};
    // int nums[] = {15,16,19,20,25,1,3,4,5,7,10,14};
    // int nums[] = {0};
    int j = 0;
    // for(j; j < 8; j++) {
        printf("start check %d\n", j);
        int i = search(nums, sizeof(nums) / sizeof(nums[0]), j);
        printf("get %d\n", i);

    // }
    return 0;
}