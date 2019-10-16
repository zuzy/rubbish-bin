#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>


int main(int argc, char *argv[])
{
    char *a = "1.2.3.232sdf";
    char tmp[32] = "\0";
    char b,c,d;
    sscanf(a, "%2"SCNu8".%2"SCNu8".%2"SCNu8, &b, &c, &d);
    printf("ver : %s\n %d.%d.%d\n", a, b, c, d);
    return 0;
}