#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int firstUniqChar(char* s) {
    int len = strlen(s);
    char *tmp = calloc(1, len + 1);
    strcpy(tmp ,s);
    int i,j;
    for(i = 0; i < len; ++i) {
        if(tmp[i] == '\0') continue;
        int en = 1;
        for(j = i+1; j < len; ++j) {
            if(tmp[j] == '\0') continue;
            if(tmp[j] == tmp[i]) {
                en = 0;
                tmp[j] = '\0';
            }
        }
        if(en) {
            free(tmp);
            return i;
        }
    }
    free(tmp);
    return -1;
}

int main(int argc, char *argv[])
{
    return 0;
}