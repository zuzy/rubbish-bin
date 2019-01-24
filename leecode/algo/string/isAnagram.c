#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int comp(const void *a, const void *b)
{
    return *(char *)a - *(char *)b;
}
#if 0
bool isAnagram(char* s, char* t) {
    int len = strlen(s);
    if(len != strlen(t)) {
        return false;
    }
    char *a = calloc(1, len + 1);
    char *b = calloc(1, len + 1);
    strcpy(a, s);
    strcpy(b, t);

    qsort(a, len, 1, comp);
    qsort(b, len, 1, comp);
    printf("a : %s\nb : %s\n", a,b);
    return strcmp(a, b) == 0;
}
#else
bool isAnagram(char* s, char* t) {
    int len = strlen(s);
    if(len != strlen(t)) {
        return false;
    }
    int i, j;
    int count = 0;
    for(i = 0; i < len; i++) {
        for(j = 0; j < len; j++){
            if(t[j] == 0) continue;
            if(s[i] == t[j]) {
                t[j] = 0;
                ++count;
                break;
            }
        }
    }
    return count == len;
}

#endif
int main(int argc, char *argv[])
{
    return 0;
}