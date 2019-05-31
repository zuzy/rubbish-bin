#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define JUDGE(n) (n <= '9' && n >= '0')
#define INT_MAX (0x80000000-1)
#define INT_MIN (-2147483648)

int myAtoi(char* str) {
    char *p = str;
    int neg = 0;
    long long int ret = 0;
    while(*p++ == ' ');
    if(*p == '-' || *p == '+') {
        if(*p == '-') {
            neg = 1;
        }
        ++p;
    }
    while(JUDGE(*p)) {
        ret *= 10;
        ret += *p - '0';
        ++p;
        if (ret > INT_MAX) break;
    }
    ret = neg? -ret: ret;
    printf("ret is %lld", ret);
    if(ret <= INT_MIN) {
        return INT_MIN;
    }
    if(ret > INT_MAX) {
        return INT_MAX;
    }
    return ret;
}

int main(int argc, char *argv[]) 
{
    char *p = "9223372036854775808";
    int a = myAtoi(p);
    printf("%s->%d\n", p, a);
}