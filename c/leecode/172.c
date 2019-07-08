#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

int64_t step(int num) 
{
    if(num < 1) {
        return 0;
    }
    int i = 1;
    int ret = 1;
    for(; i <= num; i++) {
        ret *= i;
    }
    return ret;
}

int trailingZeroes(int n){
    int ret = 0;
    int m = n;
    int step = 5;
    while(n = n / 5) {
        ret += m / step;
        step *= 5;
    }
    return ret;

}



int main(int argc, char *argv)
{
    int ret = trailingZeroes(625);
    printf("ret is %d\n", ret);
    int64_t x = step(15);
    printf("ret is %lld \n", x);

}