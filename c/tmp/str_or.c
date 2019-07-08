#include  <stdio.h>
#include <string.h>
#include <stdlib.h>

#define     POINT_DOT       "。"
#define     POINT_COMMA     "，"
#define     POINT_BANG      "！"
#define     POINT_BRANCH    "；"

static char *_points[] = {POINT_BANG, POINT_BRANCH, POINT_COMMA, POINT_DOT};

static char *_find_last_str(char *str, char *point)
{
    char *p = str;
    char *ret = NULL;
    do{
        ret = p;
        p = strstr(p, point);
        if(p) {
            if(*p & 0x80) {
                int n = 0;
                char t = *p;
                while(t & 0x80) {
                    ++n;
                    t <<= 1;
                }
                p += n;
            } else {
                ++p;
            }
        }
    }while(p != NULL);
    return ret;
}

static char *_fc_last_point(char *str)
{
    char *p = NULL;
    int i = 0;
    int size = sizeof(_points) / sizeof(_points[0]);
    printf("size i s %d\n", size);
    for(; i < size; i++) {
        char *t = _find_last_str(str, _points[i]);
        if(p < t) {
            p = t;
        }
    }
    return p;
}


int main(int argc, char *argv[])
{
    char *s = "你好小智，你好中国，你好，你好，你好！你好小小。你好";
    char *ss = strdup(s);
    char *p1 = _find_last_str(s, POINT_COMMA);
    printf("p1 is %s\n", p1);

    p1 = _find_last_str(s, POINT_BANG);
    printf("p1 is %s\n", p1);

    p1 = _find_last_str(s, POINT_DOT);
    printf("p1 is %s\n", p1);


    char *ret = _fc_last_point(ss);
    printf("ret is %p %s\n", ret, ret);
    return 0;
}