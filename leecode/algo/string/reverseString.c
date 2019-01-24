#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* reverseString(char* s) {
    int len = strlen(s);
    char *ret = calloc(1, s);
    char *ps, *pr;
    for(ps = s + len - 1, pr = ret; ps >= s; --ps, ++pr) 
        *pr = *ps;
    return ret;
}

int main(int argc, char *argv[])
{
    return 0;
}