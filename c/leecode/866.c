#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct pali_que
{
    union{
        char que[11];
        struct {
            char nil[10];
            char tail;
        };
    };
    int size;
};

static struct pali_que *_get_pali_from_(int64_t num)
{
    struct pali_que *pali = calloc(1, sizeof(struct pali_que));
    char *p = &pali->tail;
    do{
        *p-- = num % 10;
        ++pali->size;
        num /= 10;
    }while(num);
    return pali;
}

static int64_t _get_num_from(struct pali_que *pali)
{
    char *p = &pali->tail;
    int64_t num = 0;
    int size = pali->size;
    do {
        num *= 10;
        num += *p--;
    }while(--size);
    return num;
}

static int _check_palidrome(struct pali_que *pali) 
{
    int i = 0;
    char *p = &pali->tail;
    size_t size = pali->size - 1;
    size_t len = pali->size >> 1;
    for(; i < len; i++) {
        if(*(p - i) != *(p - size + i)) {
            return 0;
        }
    }
    return 1;
}

int check(int n) 
{
    if(n < 2) {
        return 1;
    }
    if(0 == n % 2) {
        return 0;
    }

    int i = 2;
    for(; (i * i) <= n; i++) {
        if(0 == n % i) {
            return 0;
        }
    }
    return 1;
}

static int64_t _find_next_palidrome(struct pali_que *pali, int check)
{
    if(!check || !_check_palidrome(pali)) {
        char *p = &pali->tail;
        char *t = p - (pali->size >> 1);
        char *t1 = t;
        if(pali->size % 2 == 0) {
            ++t1;
        }
        while(t != p && *t <= *t1) {
            *t++ = *t1--;
        }
        if(t == p && *t <= *t1) {
            *t = *t1;
            return _get_num_from(pali);
        } else {
            while(t <= p) {
                *t++ = *t1--;
            }
        }
    }
    char *p = &pali->tail;
    char *t = p - (pali->size >> 1);
    char *t1 = t;
    if(pali->size % 2 == 0) {
        ++t;
    } else {
        if(++*t <= 9 || t== p) {
            goto Tag;
        }
        *t++ = 0;
        --t1;
    }
    int i = 0;
    while((++*t1,++*t) > 9 && t != p) {
        *t = 0;
        *t1 = 0;
        ++t;
        --t1;
    }
    Tag:
    printf("*t is %d %d\n", *t, *t1);
    if(t == p) {
        if(*t > 9) {
            *t = 1;
            if(t1 != t)
                *t1-- = 0;
            else 
                *--t1 = 1;
            *t1 = 1;
            ++pali->size;
        } else {
            if(*t % 2 == 0) {
                ++*t;
                if(t1 != t)
                    ++*t1;
            }
        }
    }
    return _get_num_from(pali);
}


int primePalindrome(int N)
{
    if(N <= 2) {
        return 2;
    }
    struct pali_que *pali = _get_pali_from_(N);
    int64_t ret = N;
    int c = _check_palidrome(pali);
    for(;;) {
        if(c && check(ret)) {
            return ret;
        }
        ret = _find_next_palidrome(pali, c);
        c = 1;
        printf("ret is %ld\n ", ret);
    }
    return ret;
}

int main(int arc, char *argv[])
{
    // int s = primePalindrome(85709140);
    int s = primePalindrome(13832);
    // struct pali_que *pli = _get_pali_from_(85709140);
    // int ret = _get_num_from(pli);
    // // printf("RET IS %d\n", ret);
    // int i = 0;
    // int s = 0;
    // for(; i < 2; i++) {
    //     int r = _find_next_palidrome(pli);
    //     printf("R IS %d\n", r);
    // }
    // int s = _find_next_palidrome(pli);
    printf("return is %d\n", s);
    
    // int s = primePalindrome(200);
    printf("s is %d\n", s);
    return 0;
}