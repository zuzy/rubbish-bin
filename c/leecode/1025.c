#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int alice(int N, int select)
{
    if(N == 1) {
        return 0;
    }
    return N - select;
}

int bob(int N, int select)
{
    if(N == 1) {
        return 0;
    }
    return N - select;
}

bool check_game(int N, int person)
{
    printf("%s: %d %d\n", __func__, N, person);
    if(N == 1) {
        return false;
    }
    if(N == 2) {
        return true;
    }

    int i = 1; 
    bool ret = false;
    for(; i < N; i++) {
        if( 0 == (N % i)) {
            printf("too check %d\n", i);
            ret |= check_game(N - i, !person);
        }
        printf("ret is %d\n", ret);
        if(ret == true) {
            return ret;
        }
    }
    printf("return ?\n");
    return false;
}

#if 0
bool divisorGame(int N){
    return !(N % 2);
}
#else 
bool divisorGame(int N){
    return check_game(N, 1);
}
#endif 

int main(int argc, char *argv[])
{
    int a = 0;
    sscanf(argv[1], "%d", &a);
    printf("get %d\n", a);
    bool ret = check_game(a, 1);
    printf("return is %d\n", ret);
    return 0;
}