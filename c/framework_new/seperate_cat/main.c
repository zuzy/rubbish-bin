#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char str[1024] = "";

void cat1()
{
    char *p = str + 10;
    *p++ = 0x7d;
    *p++ = 0x01;
    *p++ = 0x01;
    strcat(p, "helo");
}


void cat()
{
    char *p = str + strlen(str);
    *p++ = 0x7d;
    *p = 0x02;
    strcat(str, "world");
}

void mvforward(char *des, int size, int step)
{
    int len = size - step;
    char *ptr = des;
    int i = 0;
    for(; i < len; i++) {
        *ptr = *(ptr + step);
        ++ptr;
    }
}

void del_null(char *des, int len)
{
    int num = 0;
    char *ptr = des;
    int size = len;
    while(*ptr++ < 32 && len-- > 0) {
        ++num;
    }
    if(len && num) {
        mvforward(des, size, num);
    }
}

void del_flag(char *des, int size)
{
    char *head = des;
    char *end = des + strlen(des);
    int flag_num = 0;
    while((end - (flag_num << 1)) >= head) {
        if(*(end - (flag_num << 1)) == 0x01 || *(end - (flag_num << 1)) == 0x02) {
            ++flag_num;
        }
        if(flag_num) {
            *end = *(end - (flag_num << 1));
        }
        --end;
    }
    if(flag_num) {
        mvforward(head , size, flag_num << 1);
    }
}
#if 0
void change_flag(char *des, int size)
{
    char *head = des;
    char *end = des + strlen(des);
    int flag_num = 0;
    char val = 0;
    while((end - flag_num) >= head) {
        val = *(end - flag_num);
        if((val != 0x01 && val != 0x02) || *(end - flag_num - 1) != 0x7d) {
            *end = *(end - flag_num);
            --end;
        } else {
            *end-- = val + 0x7c;
            ++flag_num;
        }
    }
    if(flag_num) {
        mvforward(head , size, flag_num);
    }
}
#else
// int change_flag(char *des, int size)
// {
//     if(des == NULL || size <= 0) {
//         return -1;
//     }
//     char *ptr = des;
//     char *p_mv = des;
//     int escape = 0;
//     int i = 0;
//     for(i = 0; i < size; ++i) {
//         if(escape) {
//             if(*p_mv == 0x01 || *p_mv == 0x02) {
//                 *ptr++ = *p_mv++ + 0x7c;
//                 escape = 0;
//                 continue;
//             }
//             printf("escape error %d %.02X !\n", i, *p_mv);
//             return -1;
//         }
//         if(*p_mv == 0x7d) {
//             escape = 1;
//             ++p_mv;
//             continue;
//         } else {
//             *ptr++ = *p_mv++;
//         }
//     }
//     i = ptr - des;
//     memset(ptr, 0, size - i);
//     return size - i;
// }

int change_flag(char *des, int size)
{
    char *head = des;
    char *end = des + strlen(des);
    int flag_num = 0;
    char val = 0;
    // print_common();
    while((end - flag_num) >= head) {
        val = *(end - flag_num);
        if((val != 0x01 && val != 0x02) || *(end - flag_num - 1) != 0x7d) {
            *end = *(end - flag_num);
            --end;
        } else {
            *end-- = val + 0x7c;
            ++flag_num;
        }
    }
    // print_common();
    if(flag_num) {
        mvforward(head , size, flag_num);
    }
    return flag_num;
}
#endif
int main(void)
{
    cat1();
    printf("str :%s\n", str);
    del_null(str, 1024);
    printf("str :%s\n", str);


    int i = 0;
    for(; i < 3; i++) {
        cat();
    }

    int len = strlen(str);
    printf("str is \n%s\n", str);
    for(i = 0; i < len; i++) {
        printf("%.02x ", str[i]);
    }
    printf("\n");


    // printf("==============\n");
    // mvforward(str, 1024, 1);
    // del_null(str, 1024);
    // printf("%s\n", str);


    printf("==============\n");
    // del_flag(str, 1024);
    int ret = change_flag(str, 1024);
    len = strlen(str);
    printf("str is %d\n%s\n", ret, str);
    for(i = 0; i < len; i++) {
        printf("%.02x ", str[i]);
    }
    printf("\n");
}

