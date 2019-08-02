#include <stdio.h>
#include <stdlib.h>
#include <string.h>


long long int _str2num(char *str)
{
    long long int ret = 0;
    char *p = str;
    while(*p) {
        ret *= 10;
        ret += *p - '0';
        ++p;
    }
    return ret;
}

char *num2str(long long int a, size_t len)
{
    printf("get %lld\n", a);
    char *ret_str = calloc(1, len);
    char *p = ret_str;
    if(a)
        while(a) {
            int b = a % 10;
            *p++ = b + '0';
            a /= 10;
        }
    else{
        ret_str[0] = '0';
        return ret_str;
    } 

    char *q = ret_str;
    --p;
    while(q < p){
        char t = *q;
        *q = *p;
        *p = t;
        --p;
        ++q;
    }
    return ret_str;
}

#define MAX_LEN 220
#define FINAL MAX_LEN-1
char* multiply(char* num1, char* num2) {
    char *ret = calloc(1, MAX_LEN);
    char *pr = ret + FINAL;
    unsigned int *ret_int = calloc(MAX_LEN, sizeof(int));
    unsigned int *p;
    char *p1 = num1;
    char *p2;
    int i = 1;
    while(*p1) {
        p2 = num2;
        p = ret_int + i;
        while(*p2) {
            *p += (*p1 - '0') * (*p2 - '0');
            printf("%c * %c -> %d\n", *p1, *p2, *p);
            ++p;
            ++p2;
        }
        ++p1;
        ++i;
    }

    uint *_p = ret_int;
    while(_p <p) {
        printf("%d ", *_p);
        ++_p;
    }
    printf("\n");

    --p;
    printf("ok\n");
    while(p >= ret_int) {
        printf("(%d,%d,%d) -> ", *(p-2), *(p-1), *p);
        
        if(*p > 100) {
            *(p-2) += *p / 100;
            *p = *p % 100;
        }
        if(*p >= 10) {
            *(p-1) += *p / 10;
            *p = *p % 10;
        }
        // *p += '0';
        *pr = *p + '0';
        printf("(%d,%d | %c)\n", *(p-2), *(p-1), *pr);
        --pr;
        --p;
    }
    printf("\n");
    if(*ret == '0' || *ret == 0) {
        char *r = malloc(220);
        int i = 0;
        while(i < MAX_LEN && (ret[i] == '0' || ret[i] == '\0')){
            ++i;
        }
        if(i == MAX_LEN) {
            r[0] = '0';
        } else {
            strcpy(r, ret+i);
        }
        free(ret);
        return r;
    } else {
        return ret;
    }
}

/*
"498828660196"
"840477629533"

419254329864656431168468



419251511023056431168468
*/

/*
"23108814760092"
"55183904456427981"
*/

int main(int argc, char *argv[])
{
    char *num1 = "2";
    char *num2 = "3";
    char *ret = multiply(num1, num2);
    printf("ret is %s\n", ret);
    return 0;
}