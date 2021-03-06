#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cjson_list.h"
#include "cJSON.h"
#include "list.h"

static char *_next_json(char *str)
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

static void _add_json_list(struct list_head *head, cJSON *item)
{
    st_jsonlist *ptr = (st_jsonlist *)malloc(sizeof(st_jsonlist));
    ptr->item = item;
    list_add_tail(&ptr->list, head);
}

void get_json_list(struct list_head *head, char *str)
{
    cJSON *item = cJSON_Parse(str);
    _add_json_list(head, item);
    char *p = _next_json(str);
    print_json("P %s", p);
    while(p != NULL) {
        item = cJSON_Parse(p);
        if(item == NULL) {
            break;
        }
        _add_json_list(head, item);
        p = _next_json(p);
        print_json("P %s", p);
    }
}

void free_json_list(struct list_head *head)
{
    st_jsonlist *ptr, *tmp;
    list_for_each_entry_safe(ptr, tmp, head, list) {
        list_del(&ptr->list);
        cJSON_Delete(ptr->item);
        free(ptr);
        ptr = NULL;
        print_json("++");
    }
    head = NULL;
}

int check_json_string(char *input)
{
    return (_next_json(input) != NULL);
}

#if 0
char str[] = "{\"h{hh\":123,\"bbb\":23}{\"sdf\":232,\"sdf\":{\"sdf\":23}}}";

int main(void)
{
    #if 0
    printf("str ori is %s\n", str);
    char *p = _next_json(str);
    printf("str next is %s\n", p);
    #else
    struct list_head head = LIST_HEAD_INIT(head);
    get_json_list(&head, str);
    st_jsonlist *p;
    list_for_each_entry(p, &head, list) {
        char *out = cJSON_Print(p->item);
        print_json("out is %s", out);
        free(out);
    }
    free_json_list(&head);
    #endif
    return 0;
}
#endif