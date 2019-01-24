#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int maxProfit(int* prices, int pricesSize) {
    int *p = prices;
    int *p_e = prices + pricesSize;
    int ret = 0;
    while (p < p_e) {
        while( *p >= *(p+1) && p < p_e) {
            ++p;
        }
        if(p != p_e) {
            printf("get min %d\n", *p);
            ret -= *p;
        } else {
            break;
        }
        while( *p <= *(p+1) && p < p_e) {
            ++p;
        }
        printf("get max %d\n", *p);
        if(p == p_e) {
            ret += *(p - 1);
            break;
        } else {
            ret += *p;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int nums[] = {7,1,5,3,6,4};
    int ret = maxProfit(nums, sizeof(nums) / sizeof(nums[0]));
    printf("get ret %d \n", ret);
    return 0;
}