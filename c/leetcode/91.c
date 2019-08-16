#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int _math_C(int base, int choice)
{
    if(!choice || base == choice) {
        return 1;
    }
    if(choice > (base >> 1)) {
        choice = base - choice;
    }

    int ret = 1;
    int i = 0;
    for(i = 0; i < choice; i++, base--) {
        ret *= base;
    }
    for(i = choice; i > 0 ; i--) {
        ret /= i;
    }
    return ret;
}

static int compute_double_list(int size)
{
    int i = 0, ret = 0;
    for(; size >= i; i++, size--) {
        ret += _math_C(size, i);
    }
    return ret;
}

int numDecodings(char * s)
{
    int ret = 1;
    int double_num = 0;
    int two = 0;
    char *p = s;

    // while(*p && *p++ == '0'); 
    
    if(!*p || *p == '0') return 0;

    while(*p) {
        printf(" %c %d\n", *p, double_num);
        switch (*p)
        {
            case '0': {
                if(double_num) {
                    --double_num;
                    goto Compute_times;
                }
                return 0;
            }
            case '1': {
                ++double_num;
                two = 0;
                break;
            }
            case '2': {
                ++double_num;
                two = 1;
                printf("%d\n", double_num);
                break;
            }
            case '3'...'6': {
                if(double_num) {
                    ++double_num;
                    goto Compute_times;
                }
                break;
            }
            default: {
                if(double_num) {
                    if(!two) {
                        ++double_num;
                    }
                    printf("%d\n", double_num);
                    goto Compute_times;
                }
                break;
            }
        }
        ++p;
        continue;
    Compute_times:
        printf("double_num, %d\n", double_num);
        if(double_num)
            ret *= compute_double_list(double_num);
        double_num = 0;
        ++p;
    }
    
    if(double_num) {
        ret *= compute_double_list(double_num);
    }

    return ret;
}


int main(int argc, char *argv[])
{
    int ret, i;
#if 1
    // for(i = 1; i < 7; i++) {
    //     ret = compute_double_list(i);
    //     printf("compute %d -> %d\n", i, ret);
    // }

    ret = numDecodings("110");
    printf("ret is %d\n", ret);
    ret = numDecodings("24");
    printf("ret is %d\n", ret);
#else
    for(i = 1; i < 10; i++) {
        int j = 1;
        for(; j <= i; j++) {
            ret = _math_C(i, j);
            printf("C(%d, %d) -> %d\n", i, j, ret);
        }
    }
#endif
    return 0;
}