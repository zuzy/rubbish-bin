#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    long int ms = time(NULL) * 1000;
    printf("ms : %ld\n", ms);
    sleep(1);
    ms = time(NULL) * 1000;
    printf("ms : %ld\n", ms);
    return 0;
}