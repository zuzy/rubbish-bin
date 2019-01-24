#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

static int* _sep_list_arr(char *str)
{
    char *p = NULL;
    int len = strlen(str);
    int *ret = NULL;
    if(len > 0) {
        ret = (int *)malloc(sizeof(int) * ((len >> 1) + 2));       // "1,2"
    } else {
        return NULL;
    }
    int *p_ret = ret;
    for(p = str; p - str <= len; ++p) {
        if(*p >= '0' && *p <= '9') {
            int num = 0;
            while((*p >= '0') && (*p <= '9')) {
                num *= 10;
                num += *p++ - '0';
            }
            *p_ret++ = num;
        }
    }
    *p_ret = -1;
    return ret;
}

int main(int argc, char *argv[])
{
    int *s;
    int i = 0;
    for(; i < 10; i++) {
        s = _sep_list_arr("1,0,5");
        int *p = s;
        printf("sss  %d\n",s);
        printf("get arrs: ");
        while(*p != -1) {
            printf("%d ", *p++);
        }
        printf(" %d\n", *p);
        free(s);
        s = NULL;
    }
    return 0;
}
