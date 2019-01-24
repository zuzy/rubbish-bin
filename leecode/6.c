#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char* convert(char* s, int numRows, int *l_ret) 
{
    int unit = (numRows << 1) - 2;
    int len = strlen(s);
    printf("len is %d\n", len);
    int a = len / unit;
    int b = len % unit;

    char *r;
    int line = (a + (b?1:0)) * (numRows - 1);
    printf("line is %d\n", line);
    *l_ret = numRows * line;
    printf("lret is %d \n", *l_ret);
    
    r = calloc(1, *l_ret);
    int i = 0;
    for(; i < len; ++i) {
        int x = i / unit;
        int y = i % unit;

        printf("%d %d--> %c\n", x, y, s[i]);
        if(y < numRows) {
            *(r+x*(numRows - 1) + y) = s[i];
        } else {
            *(r + (x + y - numRows + 1)*(numRows - 1) + y - numRows + 1) = s[i];
        }
    }
    return r;
}

int main(int argc, char *argv[])
{
    int l;
    char *r = convert("PAYPALISHIRING", 4, &l);
    printf("l is %d\n", l);
    int i = 0;
    for(; i < l; i++) {
        printf("%d %c   ", i, r[i]);
    }
    printf("\n");
    return 0;
}