#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool checkRecord(char * s)
{
    char *p = s;
    int num_a = 0;
    int num_l = 0;
    while(*p) {
        if('L' == *p) {
            if(++num_l > 2) {
                return false;
            }
        } else {
            num_l = 0;
            if('A' == *p) {
                if(++num_a > 1) {
                    return false;
                }
            }
        }
        ++p;
    }
    return true;
}

