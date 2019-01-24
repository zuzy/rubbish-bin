#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MIN (-(1<<31))
#define MAX ((1ll<<31)-1)

int divide(int dividend, int divisor) 
{
    if(!divisor) return MIN;
    if(!dividend) return 0;
    long long end = dividend;
    long long sor = divisor;

    int neg = 0;
    if(end < 0) {
        printf("< 0\n");
        end = -end;
        printf("%lld\n", end);
        if(sor < 0) {
            sor = -sor;
        } else {
            neg = 1;
        }
    } else {
        if(sor < 0) {
            neg = 1;
            sor = -sor;
        }
    }
    printf("(%lld / %lld) ~ (%lld, %lld)\n", end, sor, MIN, MAX);
    if(end < sor) return 0;
    long long i = 1;
    long long tmp = sor;
    
    for(; tmp < end; tmp <<= 1, i <<= 1);
    for(; tmp > end; tmp -= sor, --i);
    i = neg? -i: i;
    if(i < MIN || i > MAX) {
        return MAX;
    }
    return i;
}

// -2147483648
// -1

int main(int argc, char *argv[])
{
    int a,b;
    a = -2147483648;
    // a = -23;
    b = -1;
    int ret = divide(a, b);
    printf("%d / %d = %d\n", a, b, ret);
    return 0;
}