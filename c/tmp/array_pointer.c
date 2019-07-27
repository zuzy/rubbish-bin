#include <stdio.h>

int a[] = {1,2,3,4,5};

int main() 
{
    int *b = &a + 1;
    --b;
    printf("b is %d\n", *b);

    printf("sizeof is %ld\n", sizeof(a));
    printf("%p %p\n", a, &a);
}
