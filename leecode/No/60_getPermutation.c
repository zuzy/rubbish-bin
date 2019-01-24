#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int factorial(int in)
{
    int ret = 1;
    for(; in > 0; --in){
        ret *= in;
    }
    return ret;
}

int find_min_factorial(int n, int k)
{
    int s = 1;
    int e = n + 1;
    int mid;
    while(1) {
        mid = (s + e) / 2;
        int r = factorial(mid);
        printf("cal %d! = %d\n", mid, r);
        if(r == k) {
            printf("equal\n");
            return mid;
        }
        if(r < k && k < r * (mid + 1)) {
            return mid + 1;
        }
        if(r < k) {
            s = mid;
        } else {
            e = mid;
        }
        if(mid == 1 || mid == n) {
            return mid;
        }
        // usleep(200000);
    }
}

char* getPermutation(int n, int k) {
    char *ret = calloc(1, n + 1);
    int s = find_min_factorial(n, k);
    printf("find min %d\n", s);
    char *p = ret;
    int i = 0;
    for(; i < n - s; i++) {
        ret[i] = i + '1';
    }
    int *tmp = malloc(s * sizeof(int));

    int j = 0;
    int index = i;
    for(; j < s; j++) {
        tmp[j] = ++i;
        printf("tmp[%d] -> %d\n", j, tmp[j]);
    }
    int max = s - 1;
    printf("k -> %d\n", k);
    while(s--) {
        if(k == 0) {
            int i = max;
            while(tmp[i] == 0) {
                --i;
            }
            ret[index++] = tmp[i] + '0';
            tmp[i] = 0;
        } else {
            int f = factorial(s);
            int quo = k / f;
            int rem = k % f;
            k = rem;
            printf("%d %d %d\n", f, quo, rem);
            if(rem == 0) {
                --quo;
            }
            int i = 0;
            int j = 0;
            for(; i < max; i++) {
                if(tmp[i]) {
                    if(j == quo) {
                        break;
                    }
                    ++j;
                }
            }
            printf("j -> %d\n", j);
            ret[index++] = tmp[i] + '0';
            tmp[i] = 0;
            
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int i = 0;
    char *r;
    for(i = 1; i <= 24; i++) {
        r = getPermutation(4, i);
        printf("get %s\n", r);
    }
    return 0;
}