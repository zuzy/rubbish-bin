#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *p = malloc(0);
    printf("p -> %p\n", p);
    free(p);
    return 0;
}