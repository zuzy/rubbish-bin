#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int custom_to_hex(char *input, char *ret, int length)
{
    int retlen = 0;
    if(!input || !length) goto End;

    size_t ilen = strlen(input);
    if(ilen > 100 ||  ilen == 0) 
        goto End;
    // retlen = ilen >> 1;
    
    char *p = input;
    unsigned char *r = (unsigned char *)ret;
    unsigned char flag = 1;
    while(*p) {
        unsigned char val = 0;
        switch (*p)
        {
        case '0'...'9': {
            val = *p - '0';
            break;
        }
        case 'A'...'Z': {
            val = *p - 'A' + 0x0a;
            break;
        }
        case 'a'...'z': {
            val = *p - 'a' + 0x0a;
            break;
        }
        default:
            goto End_of_while;
        }
        if(flag) {
            *r = val;
        } else {
            *r = (*r << 4) | val;
            ++r;
            ++retlen;
        }
        if(length < (char *)r - ret) {
            return length;
        }
        flag = !flag;
        End_of_while:
        ++p;
    }
    if(!flag) {
        *r <<= 4;
        ++retlen;
    }
    End:
        return retlen;
}

int main(int argc, char *argv[])
{
    char *s = "12 34 56 78 90  ";
    char tmp[50] = {};
    int to = custom_to_hex(s, tmp, 50);
    printf("ret %s\n", s);
    printf("to %d\n", to);
    int i = 0;
    for(; i < to; i++) {
        printf(" %d: %02x ", i, tmp[i]);
    }
    printf("\n");
    return 0;
}