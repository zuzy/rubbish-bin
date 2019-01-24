#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char argv[])
{
    char *str = calloc(1, 100);
    sprintf(str, "123123123");
    sprintf(str, "123");
    printf("str Is %s\n", str);
    int i = 0;
    for (; i < 100; i++) {
        printf("%d %c\n", i, str[i]);
    }
    printf("\n");
    free(str);
    return 0;
}