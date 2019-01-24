#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */

char *a[] = {
    "",
    "",
    "abc",
    "def",
    "ghi",
    "jkl",
    "mno",
    "pqrs",
    "tuv",
    "wxyz",
    NULL,
};

void _fun_(char *input, char *dig, char **p, int *size) 
{
    if(*dig == '\0') {
        if(input) {
            char **ptr = p;
            while(*ptr) ++ptr;
            *ptr = input;
            *size = ptr - p + 1;
        } else {
            *size = 0;
        }
        return;
    }
    int i = 0;
    int index = *dig - '0';
    // printf("dig %s\n", dig);
    if(input == NULL) {
        input = calloc(1, 1);
    }
    // printf("calloc!");
    for(; i < strlen(a[index]); ++i) {
        char *tmp = calloc(1, strlen(input) + 1);
        strcpy(tmp, input);
        tmp[strlen(tmp)] = a[index][i];
        // printf("i: %d, tmp is %s\n", i, tmp);
        _fun_(tmp, dig+1, p, size);
    }
    free(input);
}

char** letterCombinations(char* digits, int* returnSize) {
    // printf("!!\n");
    char **p = (char **)calloc(1024, sizeof(char *));
    // printf("!!\n");
    _fun_(NULL, digits, p, returnSize);
    return p;
}


int main(int argc, char *argv[])
{
    char *input = "";
    int ret = 0;
    char **p = letterCombinations(input, &ret);
    char **pp = p;
    printf("get %d\n", ret);
    while(*pp) {
        printf("%s\n", *pp);
        ++pp;
    }

    return 0;
}