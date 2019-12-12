#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


/**
 * 166. 分数到小数
 * https://leetcode-cn.com/problems/fraction-to-recurring-decimal/
 */

static void _dump_list_int(long *list)
{
    printf("%s %d:\n", __func__, __LINE__);
    while(*list) {
        printf("\t%ld", *list++);
    }
    printf("\n");
}

static int _check_(long *list, long target)
{
    long *p = list;
    while(*p) {
        if(*p == target) {
            return p - list + 1;
        }
        ++p;
    }
    return 0;
}

static int _check_sign_(long *numerator, long *denominator)
{
    int ret = 0;
    if(*numerator < 0) {
        *numerator = -*numerator;
        ret = ~ret;
    }
    if(*denominator < 0) {
        *denominator = -*denominator;
        ret = ~ret;
    }
    return ret;
}

static void lnum_to_string(char *target, size_t size, long num, int sign, int force)
{
    int i = 0;
    char tmp[size + 1];
    char *p = tmp + size - 1;
    *p-- = 0;
    long quotient, remainder, tmp_num = num;
    
    for(i = 0; i < size; i++) {
        *p-- = num % 10 + '0';
        num = num / 10;
        if(!num) {
            break;
        }
    }
    ++p;
    printf("%s: p: %s\n", __func__, p);
    if(sign && (force || tmp_num)) {
        strcpy(target, "-");
    } else {
        memset(target, 0, size);
    }
    strcat(target, p);
    printf("target %s\n", target);
}

char * fractionToDecimal(int _numerator, int _denominator)
{
    long numerator = _numerator;
    long denominator = _denominator;
    int sign = _check_sign_(&numerator, &denominator);
    printf("%d %ld %ld\n", __LINE__, numerator, denominator);

    long integer = numerator / denominator;
    printf("integer is %ld\n", integer);
    numerator = numerator % denominator;
    printf("%ld, %ld\n", numerator ,denominator);

    long *remainder = (long*)calloc(sizeof(long), denominator > 65535 ? 65535: denominator);
    long *p_r = remainder;
    char *decimal = calloc(1, denominator + 3);
    char *p_dec = decimal;
    printf("%d %ld %ld\n", __LINE__, numerator, denominator);
    char *ret = NULL;
    printf("%d %ld\n", __LINE__, numerator);
    if(numerator) {
        printf("%d %ld\n", __LINE__, numerator);
        remainder[0] = numerator;
        ++p_r;
        printf("%d %ld\n", __LINE__, numerator);
        int pos = 0;
        do {
            numerator *= 10;
            *p_dec++ = numerator / denominator + '0';
            printf("%s\n", decimal);
            numerator = numerator % denominator;
            printf("numerator: %ld\n", numerator);
            pos = _check_(remainder, numerator);
            *p_r++ = numerator;
            if(numerator == 0) {
                break;
            }
            printf("pos is %d\n", pos);
        } while (pos == 0);

        size_t size = 40 + strlen(decimal);
        // ret = calloc(1, 32 + strlen(decimal) + 3);
        ret = calloc(1, size);
        if(pos) {
            *p_dec = ')';
            char *de_tmp = decimal;
            lnum_to_string(ret, size, integer, sign, 1);
            // if(sign) 
            //     sprintf(ret, "-%ld.", integer);
            // else
            //     sprintf(ret, "%ld.", integer);
            strcat(ret, ".");
            char *pr = ret + strlen(ret);
            int i = 0;
            --pos;
            for(i = 0; i < pos; i++) {
                *pr++ = *de_tmp++;
            }
            *pr = '(';
            strcat(ret, de_tmp);
        } else {
            printf("pos !!! %s\n", decimal);
            lnum_to_string(ret, size, integer, sign, 1);
            strcat(ret, ".");
            strcat(ret, decimal);
            
            // if(sign) 
            //     sprintf(ret, "-%ld.%s", integer, decimal);
            // else
            //     sprintf(ret, "%ld.%s", integer, decimal);
        }
        printf("\ndec is %s\n", decimal);
    } else {
        ret = malloc(32);
        lnum_to_string(ret, 32, integer, sign, 0);
        // if(sign && integer) 
        //     sprintf(ret, "-%ld", integer);
        // else
        //     sprintf(ret, "%ld", integer);
    }
    free(remainder);
    free(decimal);
    return ret;
}

int main(int argc, char *argv[])
{
    if(argc > 3) {
        printf("usage: %s numerator denominator\n", argv[0]);
        return 1;
    }

    int numerator = 3;
    int denominator = 17;

    switch (argc)
    {
    case 3 ... 10:
        denominator = atoi(argv[2]);
    case 2:
        numerator = atoi(argv[1]);
    case 1:
        break;
    
    default:
        break;
    }

    char *ret = fractionToDecimal(numerator, denominator);
    printf("%d / %d:\n\t%s\n", numerator, denominator, ret);
    free(ret);
    return 0;
}
