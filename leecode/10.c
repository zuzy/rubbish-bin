#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define true 1
#define false 0
#define bool int

bool isMatch(char* s, char* p) {
    char *p_s = s;
    char *p_p = p;
    while(*p_s <= 'z' && *p_s >= 'a') {
        if(*p_p == '\0') return false;

        if(*p_p == '.') {
            if(*(p_p + 1) == '*') {
                if(*(p_p + 2) == '\0') {
                    return true;
                } else {
                    p_s = strrchr(p_s, *(p_p + 2));
                    if(p_s == NULL) {
                        return false;
                    }
                    p_p += 3;
                    continue;
                }
            }
        } else {
            if(*(p_p + 1) == '*') {
                int en = 0;
                while(*p_s == *p_p) {
                    en = 1;
                    ++p_s;
                }
                if(*p_p == *(p_p + 2)) {
                    if(en) {
                        ++p_p;
                    } else {
                        return false;
                    }
                }
                p_p += 2;
                continue;
            } else {
                if(*p_p != *p_s) {
                    return false;
                }
            }
        }
        ++p_s;
        ++p_p;
    }
    if(*p_p == '\0')
        return true;
    else
        return false;
}

int main(int argc, char *argv[]) 
{
    printf("aaa a*a? %d\n", isMatch("aaa", "a*a"));

}