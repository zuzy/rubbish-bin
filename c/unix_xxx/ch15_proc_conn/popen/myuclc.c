#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    int c;
    while((c = getchar()) != EOF) {
        if(isupper(c)) {
            c = tolower(c);
        }
        if(putchar(c) == EOF) {
            printf("output error\n");
        }
        if(c == '\n') {
            fflush(stdout);
        }
    }
    exit(0);
}
