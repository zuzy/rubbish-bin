#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

bool isValid(char* s) {
    char *p = s;
    char *tmp = calloc(1, strlen(s) + 1);
    char *pt = tmp;
    while(*p) {
        switch (*p) {
            case '{' : {
                *pt++ = 3;
                break;
            }
            case '[' : {
                *pt++ = 2;
                break;
            }
            case '(' : {
                *pt++ = 1;
                break;
            }
            default: {
                if(pt-1 < tmp) {
                    free(tmp);
                    return false;
                }
                if(*p == ')' && *(pt-1) == 1) {
                    --pt;
                } else if(*p == ']' && *(pt-1) == 2) {
                    --pt;
                } else if(*p == '}' && *(pt-1) == 3) {
                    --pt;
                } else {
                    free(tmp);
                    return false;
                }
            }
        }
        ++p;
    }
    if(pt != tmp) {
        free(tmp);
        return false;
    }
    free(tmp);
    return true;
}

int main(int argc, char *argv[])
{
    char *ss = "(";
    int ret = isValid(ss);
    printf("ret is %d\n", ret);
    return 0;
}