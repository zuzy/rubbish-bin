#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN 1000

static int _check_(char *input, int len) {
    #if 0
    char *s = calloc(1, len + 1);
    memcpy(s, input, len);

    char *start, *end;
    start = s;
    end = s + len - 1;
    while(start < end) {
        if(*start++ != *end--) {
            free(s);
            return 1;
        }
    }
    free(s);
    return 0;
    #else
    char *ptr1, *ptr2;
    int half = len / 2;
    ptr1 = calloc(1, half + 1);
    ptr2 = calloc(1, half + 1);
    memcpy(ptr1, input, half);
    char *_p = input + len - 1;
    char *_pt = ptr2;
    int i;
    for(i = 0; i < half; ++i) {
        *_pt++ = *_p--;
    }
    int r = strcmp(ptr1, ptr2);
    free(ptr1);
    free(ptr2);
    return r;

    #endif
}

char* longestPalindrome(char* s) {
    char *p = s;
    int len = strlen(s);
    int i = len;
    char *ret = NULL;
    for(; i > 0; --i) {
        p = s;
        int j = 0;
        for(; j < len -i + 1; ++j) {
            if(_check_(p, i) == 0) {

                char *ret = calloc(1, i+1);
                memcpy(ret, p, i);

                return ret;
            }
            ++p;
        }
    }
    ret = calloc(1, 1);
    return ret;
}

int main(int argc, char *argv[]) {
    char *ret = longestPalindrome("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabcaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    printf("ret is %s\n", ret);
    return 0;
}