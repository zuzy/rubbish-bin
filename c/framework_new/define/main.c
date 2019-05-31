#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<string.h>

int main()
{
    #define add(n) bit|=CS##n
    int bit =0;
    int n = 5;
    printf("%d\n", bit);
}