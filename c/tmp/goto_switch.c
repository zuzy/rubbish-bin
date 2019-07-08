#include <stdio.h>

int main(int argc, char *argv[])
{
    int s = 0;
    switch (s)
    {
    case 0: {
        int a;
        goto cs3;
        break;
    }
    case 1: {
        printf("1\n");
        break;
    }

    case 2:{
        printf("2\n");
        break;
    }

    case 3:
    cs3:{
        int ss =1;
        printf("3\n");
        break;
    }
    
    default:
        break;
    }
    return 0;
}