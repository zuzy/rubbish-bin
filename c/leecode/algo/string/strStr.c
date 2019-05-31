
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int strStr(char* haystack, char* needle) {
    int len = strlen(needle);
    if(len > strlen(needle)) return -1;
    char *p = haystack;
    while(strlen(p) >= len) {
        if(memcmp(p, needle, len) == 0) {
            return p - haystack;
        }
        ++p;
    }
    return -1;
}

int main(int argc, char *argv[])
{
    char *s = "A man, a plan, a canal: Panama";
    
    
    return 0;
}