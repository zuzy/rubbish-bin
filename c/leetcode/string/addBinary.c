#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* addBinary(char* a, char* b) {
    int a_l = strlen(a);
    int b_l = strlen(b);
    int r_l = a_l > b_l ? a_l : b_l;
    int ret_l = r_l;
    char *ret = calloc(1, r_l + 1);
    int en = 0;
    
    // printf("a:%d b:%d r:%d\n", a_l, b_l, r_l);

    // usleep(100);
    for(a_l -= 1, b_l -= 1, r_l -= 1; a_l >= 0 || b_l >= 0; --a_l, --b_l, --r_l) {
        if(a_l < 0) {
            ret[r_l] = b[b_l] + en;
        } else if(b_l < 0) {
            ret[r_l] = a[a_l] + en;
        } else {
            ret[r_l] = a[a_l] + b[b_l] + en - '0';
        }
        // printf("%d %d %d --> %c\n", a_l, b_l, r_l, ret[r_l]);
        if(ret[r_l] >= '2') {
            en = 1;
            ret[r_l] -= 2;
        } else {
            en = 0;
        }
    }
    if(en) {
        char *r = malloc(ret_l + 2);
        r[0] = '1';
        strcpy(r+1, ret);
        free(ret);
        return r;
    }
    return ret;
}
/*
"1011111111110101010111111100100100011101000010001010001"
"1110101001110001001111100010001101011111110111011000101"
*/
int main(int argc, char *argv[])
{
    char *ret = addBinary("1011111111110101010111111100100100011101000010001010001", "1110101001110001001111100010001101011111110111011000101");
    printf("ret is %s\n", ret);
    free(ret);
    return 0;
}