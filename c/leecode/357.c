#include <stdio.h>
#include <stdlib.h>


int countNumbersWithUniqueDigits(int n){
    if(!n) return 1;
    if(n == 1) return 10;
    int i = 0;
    int peer = 0;
    for(; i < n; i++) {
        if(!peer) {
            peer = 9;
        } else {
            peer *= (10 - i);
        }
    }
    printf("peer is %d\n", peer);
    return peer + countNumbersWithUniqueDigits(n - 1);
}

int main(int argc, char *argv[])
{
    int input = atoi(argv[1]);
    int ret = countNumbersWithUniqueDigits(input);
    printf("result is %d\n", ret);
    return 0;
}