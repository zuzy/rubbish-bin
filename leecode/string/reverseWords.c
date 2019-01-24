#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverseWords(char *s) {
    char *p = s + strlen(s) - 1;
    char *b = s;

    while(p > b) {
        char t = *b;
        *b = *p;
        *p = t;
        --p;
        ++b;
    }
    printf("%s \n", s);
    b = s;
    while(*b) {
        char *ss = strchr(b, ' ');
        if(ss == b + 1) {
            b = ss;
            continue;
        } else if(ss == NULL) {
            ss = strlen(b) + b;
        }
        char *_p = ss - 1;
        char *_b = b;
        while(_p > _b) {
            char _t = *_p;
            *_p = *_b;
            *_b = _t;
            ++_b;
            --_p;
        }
        b = ss + 1;
    }
    b = s;
    p = s;
    while(*b == ' '){
        ++b;
    }
    while(*b) {
        if(*b == ' '){
            if(*(b + 1)) {
                if(*(b + 1) == ' ') {
                    ++b;
                    continue;
                } else {
                    *p++ = *b++;
                }
            } else {
                *b = 0;
                break;
            }
        } else {
            *p++ = *b++;
        }
    }
    *p = 0;
}

int main(int argc, char *argv[])
{
    char *s = calloc(1, 1024);
    sprintf(s, "the sky    is blue");
    sprintf(s, "1");
    reverseWords(s);
    printf("%s\n", s);
    return 0;
}