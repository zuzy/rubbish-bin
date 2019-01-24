#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int romanToInt(char* s) {
    struct st{
        int num;
        char rom;
    } roman[] = {
        {1000, 'M'},
        {500, 'D'},
        {100, 'C'},
        {50, 'L'},
        {10, 'X'},
        {5, 'V'},
        {1, 'I'},
        {0, '\0'},
    };
    struct st *rr = NULL;
    char *p = s;
    int ret = 0;
    while(*p) {
        struct st *r = roman;
        while(r->num) {
            if(r->rom == *p) {
                if(rr != NULL && r->num > rr->num) {
                    ret -= (rr->num << 1);
                    ret += r->num;
                } else {
                    rr = r;
                    ret += r->num;
                }
                break;
            }
            ++r;
        }
        ++p;
    }
    return ret;

    
}

int main(int argc, char *argv[])
{
    char *a = "MCMXCIV";
    printf("%s -> %d\n", a, romanToInt(a));
    return 0;
}