#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define HALO_JSON_DEBUG 1
#if HALO_JSON_DEBUG == 1
    #define print_json(format, arg...)   do { printf("\033[31m[halo_json]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_json(format, arg...)   do {} while (0)
#endif

static char *next_json(char *str)
{
    char *ptr = str;
    int floor = 0;
    // judge if inside the "..."
    int quotes = 0;
    while(*ptr) {
        if(*ptr++ == '{') {
            ++floor;
            break;
        }
    }
    if(!floor) {
        print_json();
        return NULL;
    }
    while(floor && *ptr) {
        switch(*ptr++) {
            case '{':{
                if(quotes)
                    break;
                ++floor;
                print_json("floor is %d", floor);
                break;
            }
            case '}':{
                if(quotes)
                    break;
                --floor;
                print_json("floor is %d", floor);            
                break;
            }
            case '"':{
                quotes = !quotes;
            }
            default:{
                break;
            }
        }
    }
    if(floor == 0) {
        return ptr;
    }
    return NULL;
}
char str[] = "{hhh:bbb,sdfdsf:{shshsh:sdfd,sdsf:324}, sdfsdf:{sss:ewr}}  {sfdsf:sss}";

int main(void)
{
    printf("str ori is %s\n", str);
    char *p = next_json(str);
    printf("str next is %s\n", p);
    return 0;
}
