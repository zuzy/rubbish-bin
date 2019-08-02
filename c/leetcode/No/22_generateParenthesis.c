#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief bfs 宽度优先搜索算法
 * 
 */

int _check_(char *input)
{
    char *p = input;
    int count = 0;
    while(*p) {
        if(*p++ == '(') {
            ++count;
        } else {
            --count;
        }
        if(count < 0) {
            return 0;
        }
    }
    // if(count) return 0;
    return 1;
}
#if 0
void _build_(char **des, int size, int m_len)
{
    if(size == 0) return;

    printf("size: %d\n", size);

    char *tmp = malloc(m_len);
    if(*des){
        strcpy(tmp, *des);
        strcat(tmp, ")");
        printf("%s\n", tmp);
        char **p = des;
        while(*p) ++p;
        *p = tmp;
        _build_(p, size-1, m_len);
        tmp = malloc(m_len);
    } 
    if(*des) {
        strcpy(tmp, *des);
    } else {
        memset(tmp, 0, m_len);
    }
    strcat(tmp, "(");
    printf("%s\n", tmp);
    if(*des) {
        strcat(*des, "(");
    } else{
        *des = tmp;
    }
    _build_(des, size - 1, m_len);
}
#else

void _build_(char **des, int left, int right, int m_len)
{
    if(left == 0 && right == 0) return;
    printf("(%d %d)\n", left, right);
    char *tmp = malloc(m_len);

    if(*des && right){
        strcpy(tmp, *des);
        strcat(tmp, ")");
        if(_check_(tmp)) {
            printf("%s\n", tmp);
            char **p = des;
            if(left) {
                while(*p) ++p;
                *p = tmp;
            } else {
                strcat(*p, ")");
            }
            _build_(p, left, right - 1, m_len);
        } else {
            free(tmp);
        }
        tmp = malloc(m_len);
    } 
    if(left == 0) {
        return;
    }
    if(*des) {
        strcpy(tmp, *des);
    } else {
        memset(tmp, 0, m_len);
    }
    strcat(tmp, "(");
    printf("%s\n", tmp);
    if(*des) {
        strcat(*des, "(");
    } else{
        *des = tmp;
    }
    _build_(des, left - 1, right, m_len);
}
#endif 
/**
 * Return an array of size *returnSize.
 * Note: The returned array must be malloced, assume caller calls free().
 */
#define SIZE (65535)
#define END (SIZE - 1)
char** generateParenthesis(int n, int* returnSize) {
    char **ret = calloc(SIZE, sizeof(char*));
    char **s = ret;
    char **e = ret + END;
    *returnSize = 0;
    _build_(ret, n, n, (n << 1) + 1);
    while(*e == NULL) {
        --e;
    }
    while(*s) {
        if(_check_(*s) == 1) {
            ++s;
            ++*returnSize;
        } else {
            if(e <= s) break;
            while(_check_(*e) == 0) {
                --e;
            }
            if(e <= s) break;
            strcpy(*s, *e);
            --e;
            ++s;
            ++*returnSize;
            if(e <= s) break;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int target = 4;
    int num;
    char **ret = generateParenthesis(target, &num);
    char **p = ret;
    int i = 0;
    while(*p) {
        printf("%d -> %s\n", i, *p++);
        ++i;
    }
    printf("end %d\n",i);
    p = ret;
    for(i = 0; i < num; i++) {
        printf("%d -> %s\n", i, *p++);
    }

    return 0;
}