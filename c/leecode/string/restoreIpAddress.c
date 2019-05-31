#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// #define printf(...) NULL
#define MAX_SIZE 3*3*3*3

static size_t to_num(const char *src, size_t len)
{
    int i;
    size_t ret = 0;
    const char *p = src;
    for(i = 0; i < len; i++) {
        ret *= 10;
        ret += *p - '0';
        ++p;
    }
    return ret;
}

static int _check_(const char *input, size_t len)
{
    if(*input == '0' && len > 1) {
        return 0;
    }
    if(len < 3 && len > 0) {
        return 1;
    } else if(len == 3) {
        size_t r = to_num(input, 3);
        return (r < 256);
    } else {
        return 0;
    }
}

static int check(const char *input, int left, char **des, int *size, char *tmp)
{
    printf("left %d input %s\n", left, input);
    if(left) {
        if(tmp == NULL) {
            tmp = calloc(1, 16);
        }
        int i = 1;
        char *pp = tmp + strlen(tmp);
        for(; i <= 3; i++) {
            if(_check_(input, i)) {
                char *p = tmp + strlen(tmp);
                strncat(tmp, input, i);
                strcat(tmp, ".");
                int l = check(input + i, left - 1, des, size, tmp);
                while(*p) {
                    *p++ = 0;
                }
            } else {
                if(left == 3) {
                    memset(tmp, 0, 16);
                }
                return 0;
            }
        }
        printf("before %s\n", tmp);
        while(*pp) {
            *pp++ = 0;
        }
        printf("after %s\n", tmp);
    } else {
        // printf("!!! %s %d\n", input, *size);

        if(_check_(input, strlen(input))) {
            des[*size] = calloc(1, 16);
            strcpy(des[*size], tmp);
            strcat(des[*size], input);
            printf("check ok %s\n", des[*size]);
            printf("tmp now is %s\n", tmp);
            ++*size;
            return 1;
        } else {
            // printf("final check failed\n");
        }
    }
    return 0;
}

char** restoreIpAddresses(char* s, int* returnSize) 
{
    char **ret = calloc(MAX_SIZE, sizeof(char *));
    if(strlen(s) < 4) {
        *returnSize = 0;
        return ret;
    }
    char **p = ret;
    char *tmp = NULL;
    check(s, 3, p, returnSize, tmp);
    return ret;
}
/*
["0.10.0.10","0.100.1.0"]
*/
int main(int argc, char *argv[])
{
    int ret = 0;
    // char *s = "123456";
    // char *s = "25525512135";
    char *s = "010010";
    // char *s = "01010";
    char **str = restoreIpAddresses(s, &ret);
    char **p = str;
    int i = 0;
    for(; *p; ++p,++i) {
        printf("%d get %s\n", i, *p);
    }

    return 0;
}