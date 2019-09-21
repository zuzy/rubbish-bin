#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
* 557. 反转字符串中的单词 III
* https://leetcode-cn.com/problems/reverse-words-in-a-string-iii/
*/

void _reverse_word(char *word, int len)
{
    if(len <= 0) return;
    char *start = word;
    char *end = word + len - 1;
    while(start < end) {
        char t = *start;
        *start = *end;
        *end = t;
        ++start;
        --end;
    }
}

char * reverseWords(char * s)
{
    char *ret = strdup(s);
    char *head, *tail;
    int space_flag = 0;

    for(head = tail = ret, space_flag = 0; *tail; ++tail) {
        if(*tail == ' ') {
            if(!space_flag) {
                _reverse_word(head, tail - head);
                space_flag = 1;
            }
        } else {
            if(space_flag) {
                head = tail;
                space_flag = 0;
            }
        }

    }

    _reverse_word(head, tail - head);
    return ret;
}

int main(int argc, char *argv[])
{
    char *target = "Let's take a   LeetCode contest";
    // char *target = "a";
    // char *target = " ";
    char *ret = reverseWords(target);
    printf("revert %s\n", ret);
    return 0;
}

