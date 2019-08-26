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


#define DECLARE_START   \
    #define _NODE_(T, N, E) __typeof__(T) N; \
    #define _NODE_ARRAY(T, S, N, E) __typeof__(T) N; \
    #define _NODE_STRUCT(T, N, E, F) __typeof__(T) N;

#define DECLARE_END     \
    #undef _NODE_STRUCT \
    #undef _NODE_ARRAY \
    #undef _NODE_

#define FORMAT_START    \
    #define _TARGET_    __typeof__(struct large) \
    #define _NODE_(T,N,E) {.name=#N,.offset=offsetof(struct large, N), .size=sizeof(T), .size_node=0, .type=E}, \
    #define _NODE_ARRAY(T, S, N, E) {.name=#N,.offset=offsetof(struct large,N), .size=sizeof(T), .size_node=sizeof(S), .type=E}, \
    #define _NODE_STRUCT(T, N, E, F) {.name=#N,.offset=offsetof(struct large, N), .size=sizeof(T), .size_node=0, .type=E, .fmt=&F, .fmt_size=sizeof(F) / sizeof(json_fmt_t)}, \

#define FORMAT_END DECLARE_END

typedef enum node_type_e{
    NUMBER,
    FLOAT,
    DOUBLE,
    A_STR,
    P_STR,
    P_INT,
    P_FLOAT,
    P_DOUBLE,
    ARRAY,
    STRUCT,
}node_type_e;

typedef struct st_json_fmt_t{
    char *name;
    size_t offset;
    size_t size;
    size_t size_node;
    node_type_e type;
    void *fmt;
    size_t fmt_size;
}json_fmt_t;

typedef struct st_node_t {
    char            *name;
    size_t          offset;
    size_t          size;
    node_type_e     type;
    struct list_head    *vertical;
    struct list_head    *list;      // parallel list
}node_t;


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

cJSON *struct_to_json(void *input, json_fmt_t *fmt, int size, char *name, cJSON *target);

#endif 