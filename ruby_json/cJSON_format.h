#ifndef __CJSON_FORMAT_H__
#define __CJSON_FORMAT_H__

#define CJSON_FORMAT_DEBUG 1
#if CJSON_FORMAT_DEBUG == 1
    #define print_jmt(format, arg...)   do { printf("\033[31m[json_fmt]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_jmt(format, arg...)   NULL
#endif

#define REQUIRE_JMT(in,tag)                 do{if(in){print_jmt("err! %s", #in); goto tag;}}while(0)
#define REQ_JSON_OBJ(des,item,tag)      cJSON_GetObjectItem(des,#item); \
                                        REQUIRE_JMT((item==NULL),tag)
#define REQ_JSON_PARSE(str,item,tag)    cJSON_Parse(str); \
                                        REQUIRE_JMT((item==NULL),tag)

#include "cJSON.h"

/**
 * @brief  if big endian enable the _BIG_ENDIAN
 */
#define _BIG_ENDIAN 0

/**
 * @brief  if jmp_over == 1, json will jmp over the key by comment
  */
#define JMP_OVER 1

/**
 * @brief  format with '[#&]i[-j][:...]'
 * @note   ':' seperate the member; 
 * @note   i will match i length of uint(8*i) to member;  
 * @note   #x will jump over x bytes;
 * @note   i-j will match j times i length of uint(8*i) array;
 * @note   &1-x will match x bytes to string, 
 * 
 * @note   k_num num of keys;
 * 
 * @note   keys ended with NULL;
  */
typedef struct st_format 
{
    char *format;
    int k_num;
    char *keys[];
}format;

/**
 * @brief  format a struct to json
 * @note   encode with format, Struct MUST be pack(1), 
 * @param  *input: the target struct
 * @param  *f: format to match
 * @param  *name: master key of result json, if null members will be added at root
 * @param  *j: root json to add; if null returen json is the root;
 * @retval the result json
 */
cJSON *format_struct_to_json(void *input, format *f, char *name, cJSON *j);

#define format_to_json_simple(input, name, json) format_struct_to_json(input, input##_format, name, json)

#endif 