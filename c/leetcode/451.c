#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * frequencySort(char * s){
    int count[128] = {0};
    char *p = s;
    int len = strlen(s);
    char *ret;

    if(len <=2) {
        return strdup(s);
    } else {
        ret = (char *)calloc(len + 1, 1);
    }

    while(*p) {
        ++count[*p++];
    }

    int i,j;
    char *ret_p = ret;

    for(i = 0; i < 128;) {
        if(!count[i]) {
            ++i;
            continue;
        }

        int max = i;
        for(j = i + 1; j < 128; j++) {
            if(count[j] && (count[j] > count[max])) {
                max = j;
            }
        }
        printf("max is %d %d\n", max, count[max]);
        int k = 0;
        for(; k < count[max]; k++) {
            *ret_p++ = (char)max;
        }

        count[max] = 0;
        
    }
    return ret;
}

int main(int argc, char *argv[])
{
    char *ret = frequencySort("Aabb");
    printf("ret is %s\n", ret);
    return 0;
}