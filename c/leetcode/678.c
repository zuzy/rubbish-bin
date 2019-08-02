#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

/**
 * @brief ( * )
 * 
 */

bool check(char *s, int *layer)
{
    printf("check %s\n", s);
    char *p = s;
    int ret = 0;
    while(*p) {
        if(*p == '(') {
            ++*layer;
        } else if(*p == ')') {
            --*layer;
        } else if(*p == '*') {
            ++p;
            int layer_p = *layer + 1;
            int layer_m = *layer - 1;

            ret |= check(p, &layer_p);
            ret |= check(p, &layer_m);
            ret |= check(p, layer);
            printf("%s %d %d %d\n", p, layer_p, layer_m, *layer);
            return ret;
            break;
        }
        if(*layer < 0) {
            printf("%s layer %d < 0\n", p, *layer);
            return false;
        }
        ++p;
    }
    if(!*layer) {
        return true;
    }
    return false;
}

bool checkValidString(char * s)
{
    int layer = 0;
    return check(s, &layer);
}

int main(int argc, char *argv[])
{
    // char *s = "(*(((***))";
    // char *s = "(())((())()()(*)(*()(())())())()()((()())((()))(*";
    // char *s = "*";
    char *s = "((()))()(())(*()()())**(())()()()()((*()*))((*()*)";
    bool ret = checkValidString(s);
    printf("ret is %d\n", ret);
    return 0;
}