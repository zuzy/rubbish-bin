#ifndef __CJSON_LIST_H__
#define __CJSON_LIST_H__

#include "list.h"
#include "cJSON.h"

#define HALO_JSON_DEBUG 1
#if HALO_JSON_DEBUG == 1
    #define print_json(format, arg...)   do { printf("\033[31m[halo_json]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_json(format, arg...)   do {} while (0)
#endif

typedef struct _st_jsonlist
{
    struct  list_head list;
    cJSON   *item;    
}st_jsonlist;

void get_json_list(struct list_head *head, char *str);

void free_json_list(struct list_head *head);

#endif