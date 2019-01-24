#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _JUDGE(n) ((n>(0x80000000-1)) || (n < -0x80000000))

int reverse(int x) {

    int neg = 0;
    if(x < 0) {
        neg = 1;
    }
    long long int ret = 0;
    while(x) {
        int a = x % 10;
        ret *= 10;
        ret += a;
        x = x / 10;
    }
    printf("%ld\n", ret);
    // ret = neg ? -ret: ret;
    if(ret > 0x80000000-1 || ret < (-2147483648)) {
        printf("out of range\n");
        return 0;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int s;
    while(scanf("%d", &s)) {
        int ret = reverse(s);
        printf("%d -> %d\n", s, ret);
        usleep(100000);
    }
    return 0;
}