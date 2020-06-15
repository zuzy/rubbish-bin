#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *a = malloc(10);
    sleep(1);
    free(a);
    return 0;
}