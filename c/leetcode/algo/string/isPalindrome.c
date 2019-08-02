
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int _check_(char c)
{
    // printf("check %c\n", c);
    if(c >= '0' && c <= '9') {
        return 1;
    }
    if(c >= 'A' && c <= 'Z') {
        return 2;
    }
    if(c >= 'a' && c <= 'z') {
        return 3;
    }
    return 0;
}

bool isPalindrome(char* s) {
    int len = strlen(s);
    char *ps, *pe;
    for(ps = s, pe = ps + len - 1; pe > ps; ++ps, --pe) {
        int sf;
        while(*ps) {
            sf = _check_(*ps);
            if(sf == 0) {
                ++ps;
            } else {
                break;
            }
        }
        // for(sf = 0; sf == 0; ++ps) sf = _check_(*ps);
        int ef;
        while(*pe) {
            ef = _check_(*pe);
            if(ef == 0) {
                --pe;
            } else {
                break;
            }
        }
        // for(ef = 0; ef == 0; --pe) ef = _check_(*pe);
        // printf("sf: %d, ef: %d\n", sf, ef);
        if(sf != ef) {
            if(ef == 2) {
                if(*ps != tolower(*pe)) {
                    printf("1 %c %c\n", *ps, *pe);
                    return false;
                }
            } else if(sf == 2) {
                if(*pe != tolower(*ps)) {
                    printf("2 %c %c\n", *ps, *pe);
                    return false;
                }
            } else {
                printf("3 %c %c\n", *ps, *pe);
                return false;
            }
        } else {
            if(*pe != *ps) {
                printf("4 %c %c\n", *ps, *pe);
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    char *s = "A man, a plan, a canal: Panama";
    bool r = isPalindrome(s);
    printf("ret is %d\n", r);
    return 0;
}