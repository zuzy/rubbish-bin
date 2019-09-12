#include <stdio.h>
#include <stdlib.h>

/**
 * https://leetcode-cn.com/problems/minimum-size-subarray-sum/
 * 209. 长度最小的子数组
 * */


int minSubArrayLen(int s, int* nums, int numsSize){
    int i, j, leng = 0, min = 0;
    int sum = 0;
    for(i = 0; i < numsSize; i++) {
        sum += nums[i];
        if(sum >= s) {
            goto Find;
        }
    }
    return 0;

    Find: {
        if(i == 0) {
            // printf("just one in\n");
            return 1;
        }
        min = i + 1;
        // printf("sum first %d\n", sum);
        for(j = 0; j < numsSize; j++) {
            sum -= nums[j];
            // printf("sum sub to %d\n", sum);
            if(sum >= s) {
                leng = i - j;
                goto InRange;
            }

            for(++i; i < numsSize; i++) {
                sum += nums[i];
                // printf("sum add to %d\n", sum);
                if(sum >= s) {
                    // printf("i:%d, j:%d\n", i, j);
                    leng = i - j;
                    goto InRange;
                }
            }
            return min;

            InRange:
            if(leng == 1) {
                // printf("just one in\n");
                return 1;
            }
            if(leng < min) {
                min = leng;
            }
            // printf("min size is %d\n", min);
        }

    }
    return min;
}


