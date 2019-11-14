#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

// int list[] = {1, 2, 3, 4};
int a = 1, b = 2, c = 3, d = 4;
int list[] = {a, b, c, d};


// int *a = list;
// int *b = &list[1];
// int *c = &list[2];
// int *d = &list[3];


int main(int argc, char *argv[])
{
    char *a = "1.2.3.232sdf";
    char tmp[32] = "\0";
    char b,c,d;
    sscanf(a, "%2"SCNu8".%2"SCNu8".%2"SCNu8, &b, &c, &d);
    printf("ver : %s\n %d.%d.%d\n", a, b, c, d);
    return 0;
}