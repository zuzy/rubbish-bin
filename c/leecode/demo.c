#include<stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int a[5] = {1,2,3,4,5};
    printf("%p, %p, %p, %p\n", a, &a, a+1, &a+1);

    printf("%d, %d\n", *(a+1), *(*(&a + 1) - 1));
    return 0;
}