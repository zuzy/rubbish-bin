#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *_fun(int x, char b, char f, char l)
{
    char *ret = calloc(1, 10);
    char *tmp = calloc(1, 10);
    char *q = tmp;
    char *p = ret;
    switch(x) {
        case 3: *p++ = b;
        case 2: *p++ = b;
        case 1: *p++ = b; break;
        case 4: *p++ = b;
        case 5: *p++ = f; break;
        case 8: *q++ = b;
        case 7: *q++ = b;
        case 6: *q++ = b; break;
        case 9: *p++ = b;
                *p++ = l;
                break;
        case 0:
        default: {
            free(tmp);
            return ret;
        } 
    }
    if(q != tmp) {
        *p++ = f;
        strcat(p, tmp);
    }
    free(tmp);
    return ret;
}

char* intToRoman(int num) {
    
    char *ret = calloc(1, 128);
    int thou = num / 1000;
    int i = 0;
    for(; i < thou; ++i) {
        strcat(ret, "M");
    }

    int han = (num / 100) % 10;
    char *tmp = _fun(han, 'C', 'D', 'M');
    strcat(ret, tmp);
    free(tmp);
    int ten = (num / 10) % 10;
    tmp = _fun(ten, 'X', 'L', 'C');
    strcat(ret, tmp);
    free(tmp);
    int unit = num % 10;
    tmp = _fun(unit, 'I', 'V', 'X');
    strcat(ret, tmp);
    free(tmp);
    return ret;
    
}