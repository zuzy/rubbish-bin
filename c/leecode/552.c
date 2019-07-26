#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if 0
static int _check_na_p(int n);
static int _check_na_l(int n);
static int _check_na_ll(int n);
static int _check_a_p(int n);
static int _check_a_a(int n);
static int _check_a_l(int n);
static int _check_a_ll(int n);

int (*callback[]) (int) = {
    _check_na_p,
    _check_na_l,
    _check_na_ll,
    _check_a_p,
    _check_a_a,
    _check_a_l,
    _check_a_ll,
    NULL,
};

static int _check_na_p(int n) {
    if(n == 2) {
        return 2;
    } else {
        return _check_na_p(n - 1) + _check_na_l(n - 1) + _check_na_ll(n - 1);
    }
}

static int _check_na_l(int n) {
    if(n == 2) {
        return 1;
    } else {
        return _check_na_p(n - 1);
    }
}

static int _check_na_ll(int n) {
    if(n == 2) {
        return 1;
    } else {
        return _check_na_l(n - 1);
    }
}


static int _check_a_p(int n) {
    if(n == 2) {
        return 1;
    } else {
        return _check_a_a(n - 1) + _check_a_p(n - 1) + _check_a_l(n - 1) + _check_a_ll(n - 1);
    }
}

static int _check_a_a(int n) {
    if(n == 2) {
        return 2;
    } else {
        return _check_na_p(n - 1) + _check_na_l(n - 1) + _check_na_ll(n - 1);
    }
}

static int _check_a_l(int n) {
    if(n == 2){
        return 1;
    } else {
        return _check_a_a(n - 1) + _check_a_p(n - 1);
    }
}

static int _check_a_ll(int n) {
    if(n == 2) {
        return 0;
    } else {
        return _check_a_l(n - 1);
    }
}

int checkRecord(int n){
    if(n == 1) {
        return 3;
    }
    if(n == 2) {
        return 8;
    }
    int i = 0;
    int ret = 0;
    while(callback[i]) {
        ret += callback[i](n);
        ++i;
    }
    return ret;
}



#else
int checkRecord(int n){
    if(n == 1) {
        return 3;
    }
    if(n == 2) {
        return 8;
    }

    int64_t na_p[2] = {2, 0};
    int64_t na_l[2] = {1, 0};
    int64_t na_ll[2] = {1, 0};
    int64_t a_p[2] = {1, 0};
    int64_t a_a[2] = {2, 0};
    int64_t a_l[2] = {1, 0};
    int64_t a_ll[2] = {0, 0};
    int64_t mod = 1000000007;

    int i = 2;
    for(; i < n; i++) {
        na_p[1] = (na_p[0] + na_ll[0] + na_l[0]) % mod;
        na_l[1] = na_p[0];
        na_ll[1] = na_l[0];
        a_p[1] = (a_a[0] + a_p[0] + a_l[0] + a_ll[0]) % mod;
        a_a[1] = (na_p[0] + na_l[0] + na_ll[0]) % mod;
        a_l[1] = (a_a[0] + a_p[0]) % mod;
        a_ll[1] = a_l[0];

        na_p[0] = na_p[1];
        na_l[0] = na_l[1];
        na_ll[0] = na_ll[1];
        a_p[0] = a_p[1];
        a_a[0] = a_a[1];
        a_l[0] = a_l[1];
        a_ll[0] = a_ll[1];
    }
    return (na_p[0] + na_l[0] + na_ll[0] + a_p[0] + a_a[0] + a_l[0] + a_ll[0]) % mod;
}
#endif

int main(int argc, char *argv[]) 
{
    return 0;
}