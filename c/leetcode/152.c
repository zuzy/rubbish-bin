#include <stdio.h>
#include <stdlib.h>

int maxProduct(int* nums, int numsSize)
{
    switch (numsSize)
    {
        case 0: return 0;
        case 1: return nums[0];
        case 2: return nums[0] * nums[1];
        default: break;
    }
    typedef struct _st_three {
        int prev;
        int mid;
        int next;
    } three_t;
    three_t *tmp = (three_t *) nums;
    int index, n = numsSize / 3;
    int mul = tmp->prev * tmp->mid;

    for(index = 0; index < n; index++) {
        int m;
        if(tmp[index].next > tmp[index].prev) {
            m = tmp[index].next * tmp[index].mid;
        } else {
            m = tmp[index].prev * tmp[index].mid;
        }
        if(m > mul) {
            mul = m;
        }
    } 
    switch(numsSize - n * 3) {
        case 0: return mul;
        case 1: {
            int m = nums[numsSize - 1] * nums[numsSize - 2];
            return m > mul ? m : mul;
        }
        default: {
            int m;
            if(nums[numsSize - 1] > nums[numsSize - 3]) {
                m = nums[numsSize - 1] * nums[numsSize - 2];
            } else {
                m = nums[numsSize - 3] * nums[numsSize - 2];
            }
            return m > mul ? m : mul;
        }
    }
    return mul;
}

