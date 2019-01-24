#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* convert(char* s, int numRows) {
    if(numRows < 0) {
        return NULL;
    }
    if(numRows == 0) {
        return calloc(1, 1);
    }
    if(numRows == 1) {
        return strdup(s);
    }

    int len = strlen(s);
    int unit = numRows * 2 -2;
    int unit_row = numRows - 1;
    int unit_num = len / unit + 1;
    int unit_size = unit_row * unit_num;
    int size = numRows * unit_size;
    // printf("size is %d\n", size);
    char *tmp = calloc(1, size);
    char *pt = tmp;
    char *ps = s;
    int i;
    int tu;
    for(i = 0; *ps; ++ps, ++i) {
        int a, b, c, d;
        a = i / unit;
        b = i % unit;
        if(b) {
            c = b / numRows;
            if(c)
                d = b % numRows;
            else
                d = 0;
        } else {
            c = d = 0;
        }

        if(c) {
            *(tmp + numRows * (1 + d + a * unit_row) + numRows - 2 - d) = *ps;
        } else {
            *(tmp + numRows * a * unit_row + b) = *ps;
        }
    }

    int _i = 0;
    int _j = 0;

    // for(; _i < unit_size; _i++) {
    //     _j = 0;
    //     for(; _j < numRows; _j++) {
    //         char a = *(tmp + _i * numRows + _j);
    //         if(a) {
    //             printf("%c", a);
    //         } else {
    //             printf("0");
    //         }
    //     }
    //     printf("\n");
    // }

    // _j = 0;
    char *ret = calloc(1, strlen(s) + 1);
    char *pr = ret;
    for(; _j < numRows; _j++) {
        _i = 0;
        for(; _i < unit_size; _i++) {
            char a = *(tmp + _i * numRows + _j);
            if(a) {
                // printf("%c", a);
                *pr++ = a;
            } else {
                // printf("0");
            }
        }
        // printf("\n");
    }
    free(tmp);
    return ret;
}

int main(int argc, char *argv[])
{
    char *a = "PAYPALISHIRING";

    char *r = convert(a, 2);
    printf("ret %s\n", r);
    return 0;
}