#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int maxArea(int* height, int heightSize) {
    #if 0
    int i,j;
    int max = 0;
    for(i = 0; i < heightSize; i++) {
        for(j = i + 1; j < heightSize; j++) {
            long int tmp = height[i] > height[j] ? height[j] : height[i];
            tmp  = tmp * (j - i);
            max = tmp > max ? tmp: max;
        }
    }
    return max;
    #else
    int *p_s = height;
    int *p_e = height + heightSize - 1;
    int max = 0;
    while(p_e > p_s) {
        int small;
        int len = p_e - p_s;
        if(*p_s > *p_e) {
            --p_e;
            small = *p_e;
        } else {
            ++p_s;
            small = *p_s;
        }
        int tmp = small * len;
        max = tmp > max? tmp: max;
    }
    #endif
}


int main(int argc, char *argv[]) 
{
    int *a = {1,8,6,2,5,4,8,3,7};

    return 0;
}