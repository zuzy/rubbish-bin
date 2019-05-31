
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define DEBUG NULL
char *func(char *a) {
    printf(" intput %s\n", a);
    int len = strlen(a);
    char *ret = calloc(1, len<<1);
    DEBUG;
    int count;
    char *p = a;
    char *pr = ret;
    int i = 0;
    
    while(*p) {
        DEBUG;
        count = 1;
        while(*p == *(p+1)) {
            ++p;
            ++count;
        }
        *pr++ = count + '0';
        *pr++ = *p;
        ++p;
    }
    DEBUG;
    // free(a);
    return ret;
}

char* countAndSay(int n) {
    if(n > 1) {
        return  func(countAndSay(n - 1));
    } else return "1";
}

int main(int argc, char *argv[])
{
    char *s = countAndSay(2);
    printf(" ret is %s\n", s);
    
    
    return 0;
}