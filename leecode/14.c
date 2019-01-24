#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *_find_(char *a, char *b)
{
    char *pa = a;
    char *pb = b;
    while(*pa && *pb && *pa == *pb) {
        ++pa;
        ++pb;
    }
    return pa;
}

char* longestCommonPrefix(char** strs, int strsSize) {
    char *p = calloc(1, strlen(*strs) + 1);
    strcpy(p, *strs);
    int i = 1;
    for(; i < strsSize; i++) {
        char *s = _find_(p, strs[i]);
        if(*s) {
            *s = 0;
        }
    }
    return p;
}
int main(int argc, char *argv[])
{
    char *a = "MCMXCIV";
    printf("%s -> %d\n", a, romanToInt(a));
    return 0;
}