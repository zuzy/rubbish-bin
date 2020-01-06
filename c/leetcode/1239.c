#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int word_list[26];
    int repeation;
    int size;
} t_char_list;

static t_char_list check_string(char *str)
{
    t_char_list ret = {};
    char *p = str;
    int index = 0;
    while(*p) {
        index = *p - '0';
        ++ret.word_list[index];
        if(ret.word_list[index] > 1) {
            ret.repeation = 1;
        }
        ++ret.size;
    }
    return ret;
}

int maxLength(char ** arr, int arrSize)
{
    t_char_list list[arrSize];
    int i;
    for(i = 0; i < arrSize; i++) {
        list[i] = check_string(arr[i]);
    }
    return 0;
}