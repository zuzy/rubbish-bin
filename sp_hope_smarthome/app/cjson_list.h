#ifndef __CJSON_LIST_H__
#define __CJSON_LIST_H__

#include "list.h"
#include "cJSON.h"

#define HALO_JSON_DEBUG 0
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

/** 
 * @brief  seperate several json strings to a json head list!
 * @note   example inside the code
 * @param  *head: des list; need inition before
 * @param  *str: target string to be seperated
 * @retval -1 -> input args error; -2 -> parse json ok but add to list failed; 0 -> success
 */
int get_json_list(struct list_head *head, char *str);

/** 
 * @brief  free the list of json
 * @note   free the list
 * @param  *head: target list
 * @retval None
 */
void free_json_list(struct list_head *head);

/** 
 * @brief  check whether the sting has a string with json format inside
 * @note   check if json inside the string
 * @param  *input: target string
 * @retval 1->with json inside, 0->without
 */
int check_json_string(char *input);

#endif