#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <zlib.h>

int main(int argc, char *argv[])
{
    unsigned char a[50] = "Hello, world!00000001111111111111100000";
    unsigned char b[50];
    unsigned char c[50];

    uLong ucompSize = strlen(a) + 1; // "Hello, world!" + NULL delimiter.
    uLong compSize = compressBound(ucompSize);
    printf("%lu -> %lu\n", ucompSize, compSize);

    // Deflate
    memset(b, 0, sizeof(b));
    compress((Bytef *)b, &compSize, (Bytef *)a, ucompSize);
    printf("%lu -> %lu\n", ucompSize, compSize);
    printf("b[%lu]%s\n", strlen(b), b);
    int i = 0;
    for( ; i < compSize; ++i) {
        printf("%.02X ", b[i]);
    }
    printf("\n");
    for( ; i < 50; ++i) {
        printf("%02X ", b[i]);
    }
    printf("\n");

    // Inflate
    uncompress((Bytef *)c, &ucompSize, (Bytef *)b, compSize);
    printf("%lu -> %lu\n", ucompSize, compSize);
    printf("c[%lu]%s\n", strlen(c), c);

    return 0;
}