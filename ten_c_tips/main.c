#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int number = 0;
    
    while(scanf("%d", &number) != EOF) {
        printf("get num %d\n", number);
        printf("compute %d\n", number?:1);
    }
    return 0;
}
