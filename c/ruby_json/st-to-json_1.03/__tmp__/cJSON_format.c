#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <alloca.h>
#include "cJSON_format.h"
#include "cJSON.h"


static int get_num_from_string(char *input)
{
    if(input == NULL || *input == '\0') {
        return -1;
    }
    int n = 0;
    char *p = input;
    while(*p && (*p > '9' || *p < '0')) {
        ++p;
    }
    while(*p && *p <= '9' && *p >= '0') {
        n *= 10;
        n += *p - '0';
        ++p;
    }
    return n;
}

cJSON *struct_to_json(void *input, json_fmt_t *fmt, int length, char *key, cJSON *target)
{
    if(!input || !fmt || length <= 0) {
        return NULL;
    }
    
    int i = 0;
    cJSON *root = NULL;
    cJSON *item = NULL;

    REQUIRE_JMT((root = cJSON_CreateObject()) == NULL, Error);
    if(target && key && *key) {
        cJSON_AddItemToObject(target, key, root);
    }
    for(i = 0; i < length; i++) {
        size_t offset = fmt[i].offset;
        size_t size = fmt[i].size;
        char *name = fmt[i].name;
        size_t size_node = fmt[i].size_node;

        size_t count = 1;        
        print_jmt("Array of str, size is %lu, offset %lu, size_node %lu", size, offset, size_node);
        if(size_node) {
            count = size / size_node;
            int j;
            REQUIRE_JMT((item = cJSON_CreateArray()) == NULL, Error);
            for(j = 0; j < count; j++) {
                switch(fmt[i].type) {
                    case NUMBER: {
                        int64_t tmp = 0;
                        memcpy(&tmp, input+offset, size_node);
                        cJSON_AddItemToArray(item, cJSON_CreateNumber(tmp));
                        break;
                    }
                    case FLOAT: {
                        float tmp = *(float*)(input + offset);
                        cJSON_AddItemToArray(item, cJSON_CreateNumber(tmp));
                        break;
                    }
                    case DOUBLE: {
                        double tmp = *(double*)(input + offset);
                        cJSON_AddItemToArray(item, cJSON_CreateNumber(tmp));
                        break;
                    }
                    case A_STR: {
                        cJSON_AddItemToArray(item, cJSON_CreateString(input + offset));
                        break;
                    }
                    case P_STR: {
                        char **p = input + offset;
                        cJSON_AddItemToArray(item, cJSON_CreateString(*p));
                        break;
                    }
                    case P_INT: {
                        int **p = input + offset;
                        cJSON_AddItemToArray(item, cJSON_CreateNumber(**p));
                        break;
                    }
                    case P_FLOAT: {
                        float **p = input + offset;
                        cJSON_AddItemToArray(item, cJSON_CreateNumber(**p));
                        break;
                    }
                    case P_DOUBLE: {
                        double **p = input + offset;
                        cJSON_AddItemToArray(item, cJSON_CreateNumber(**p));
                        break;
                    }
                    case STRUCT: {
                        break;
                    }
                    default:{
                        break;
                    }
                }
                offset += size_node;
            }
            cJSON_AddItemToObject(root, name, item);
        } else {
            switch(fmt[i].type) {
                case NUMBER: {
                    int64_t tmp = 0;
                    memcpy(&tmp, input+offset, size);
                    cJSON_AddNumberToObject(root, name, tmp);
                    break;
                }
                case FLOAT: {
                    float tmp = *(float*)(input + offset);
                    cJSON_AddNumberToObject(root, name, tmp);
                    break;
                }
                case DOUBLE: {
                    double tmp = *(double*)(input + offset);
                    cJSON_AddNumberToObject(root, name, tmp);
                    break;
                }
                case A_STR: {
                    print_jmt("Array of str, size is %lu, offset %lu", size, offset);
                    cJSON_AddStringToObject(root, name, input + offset);
                    // char *tmp = malloc(size + 1);
                    // snprintf(tmp, size, "%s", input + offset);
                    // cJSON_AddStringToObject(root, name, tmp);
                    // free(tmp);
                    break;
                }
                case P_STR: {
                    print_jmt("Pointer of str, size is %lu, offset %lu", size, offset);
                    char **p = input + offset;
                    print_jmt("tmp is %p %p; %p %s", p,input + offset, *p, *p);
                    cJSON_AddStringToObject(root, name, *p);
                    break;
                }
                case P_INT: {
                    int **p = input + offset;
                    print_jmt("Pointer of str, size is %lu, offset %lu", size, offset);
                    print_jmt("tmp is %p %p; %p %s", p, input + offset, *p, (char *)*p);
                    if(!*p) {
                        cJSON_AddStringToObject(root, name,"nil");
                    } else {
                        cJSON_AddNumberToObject(root, name, **p);
                    }
                    break;
                }
                case P_FLOAT: {
                    float **p = input + offset;
                    print_jmt("Array of str, size is %lu, offset %lu", size, offset);
                    if(!*p) {
                        cJSON_AddStringToObject(root, name,"nil");
                    } else {
                        cJSON_AddNumberToObject(root, name, **p);
                    }
                    break;
                }
                case P_DOUBLE: {
                    double **p = input + offset;
                    print_jmt("Array of str, size is %lu, offset %lu", size, offset);
                    if(!*p) {
                        cJSON_AddStringToObject(root, name,"nil");
                    } else {
                        cJSON_AddNumberToObject(root, name, **p);
                    }
                    break;
                }
                case STRUCT: {
                    void *p = input + offset;
                    print_jmt("tmp is %p %p; %p %s", p,input + offset, p, (char *)p);
                    struct_to_json(input + offset, fmt[i].fmt, fmt[i].fmt_size, name, root);
                    // struct_to_json(intput + offset, )
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    Error:
    return root;
}
