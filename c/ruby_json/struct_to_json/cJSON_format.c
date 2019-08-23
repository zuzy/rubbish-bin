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

static uint64_t _buffer_to_num(char *input, int num)
{
    uint8_t *buf = (uint8_t *)input;
    uint64_t ret = 0;
    int j = 0;

    for( ; j < num; ++j, ++buf) {
#if _BIG_ENDIAN == 1
        ret <<= 8;
        ret += *buf++;
#else
        ret += *buf << (8 * j);
        // print_jmt("ret %lu, num %d 8<<num %d", ret, j, 8 * j);
#endif
    }
    return ret;
}
/**
 * @brief  format to print to json!
 * @note   if k_num < 0, format will match until keys ended with NULL;
 */


cJSON *format_struct_to_json(void *input, format *f, char *name, cJSON *j)
{
    if(f->format == NULL || *f->format == '\0') {
        return NULL;
    }
    int end = 0;
    int k_num = f->k_num;
    print_jmt("%s", f->format);

    cJSON *root = NULL;
    cJSON *item = NULL;
    cJSON *array = NULL;

    if(j != NULL ) {
        if(name && *name) {
            REQUIRE_JMT((item = root = cJSON_CreateObject()) == NULL, Error);
            cJSON_AddItemToObject(j, name, item);
        } else {
            item = root =  j;
        }
    } else if(name && *name) {
        REQUIRE_JMT((root = cJSON_CreateObject()) == NULL, Error);
        REQUIRE_JMT((item = cJSON_CreateObject()) == NULL, Error);
        cJSON_AddItemToObject(root, name, item);
    } else {
        REQUIRE_JMT((item = root = cJSON_CreateObject()) == NULL, Error);
    }

    char *ptr, *des_p;
    des_p = input;
    char *des = calloc(strlen(f->format) + 1, 1);
    strcpy(des, f->format);

    int i;
    #if JMP_OVER == 0
    for(ptr = strtok(des, ":"), i = 0; ptr && *ptr; ptr = strtok(NULL, ":"), ++i) {
    #else
    for(ptr = strtok(des, ":"), i = 0; ptr && *ptr; ptr = strtok(NULL, ":")) {
    #endif
        // print_jmt("%d:%s -> %d", i, ptr, get_num_from_string(ptr));
        int num  = get_num_from_string(ptr);
        // print_jmt("len is %d", num);
        if(*ptr == '#') {
            print_jmt("jmp over %d", num);
            des_p += num;
            continue;
        }else {
            uint64_t ret;
            cJSON *tmp = NULL;
            /**
             * @brief  match char[] in the truct;
             * @note   cannot dispatch char * in the struct; length MUST be fixed
             */
            if(*ptr == '&') {
                char *len_str = strchr(ptr, '-');
                int str_len = get_num_from_string(len_str);
                char *str_tmp = calloc(str_len, 1);
                strncpy(str_tmp, des_p, str_len);
                tmp = cJSON_CreateString(str_tmp);
                des_p += str_len;
                free(str_tmp);
            } else {
                if(strchr(ptr, '-')) {
                    char *arr_str = strchr(ptr, '-');
                    int arr_size = get_num_from_string(arr_str);
                    // print_jmt("get arr size is %d", arr_size);
                    tmp = cJSON_CreateArray();
                    int i;
                    for(i = 0; i < arr_size; i++) {
                        ret = _buffer_to_num(des_p, num);
                        des_p += num;
                        cJSON_AddItemToArray(tmp, cJSON_CreateNumber(ret));
                    }
                } else {
                    ret = _buffer_to_num(des_p, num);
                    des_p += num;
                    // print_jmt("get num %lu", ret);
                    tmp = cJSON_CreateNumber(ret);
                }
            }

            /**
             * @brief  judge the length of key;
             */
            if(k_num >= 0) {
                if(f->keys[i] == NULL) {
                    k_num = -1;
                } else {
                    if(i < k_num) {
                        cJSON_AddItemToObject(item, f->keys[i], tmp);
                    } else {
                        // print_jmt("array");
                        if(array == NULL) {
                            print_jmt("array init");
                            array = cJSON_CreateArray();
                            cJSON_AddItemToObject(item, "<def_array>",array);
                        }
                        cJSON_AddItemToArray(array, tmp);
                    }
                }
            }
            if(k_num < 0) {
                if(end == 0) {
                    if(f->keys[i] == NULL) {
                        end = 1;
                    }
                }
                if(end) {
                    if(array == NULL) {
                        print_jmt("array init");
                        array = cJSON_CreateArray();
                        cJSON_AddItemToObject(item, "<def_array>",array);
                    }
                    cJSON_AddItemToArray(array, tmp);
                } else {
                    cJSON_AddItemToObject(item, f->keys[i], tmp);
                }
            } 
            #if JMP_OVER == 1
            ++i;
            #endif
        }
    }
    return root;

    Error:{
        if(root != NULL ) {
            cJSON_Delete(root);
        }
        return NULL;
    }
}


#if 0 // test!!
/**
 * @brief  pack(1) is necessary;
 */
#pragma pack(1)
struct st_data
{
    uint8_t a;
    uint32_t b;
    uint32_t c[3];
    char byby[7];
    uint8_t a_1;
    uint32_t b_1;
    uint32_t c_1[30];
    uint8_t a_2;
    uint32_t b_2;
    uint32_t c_2[3];
    uint8_t a_3;
    uint32_t b_3;
    uint32_t c_3[3];
    uint8_t a_4;
    uint32_t b_4;
    uint32_t c_4[3];
    uint8_t a_5;
    uint32_t b_5;
    uint32_t c_5[3];
    uint8_t a_6;
    uint32_t b_6;
    uint32_t c_6[3];
    uint8_t a_7;
    uint32_t b_7;
    uint32_t c_7[3];
    uint8_t a_8;
    uint32_t b_8;
    uint32_t c_8[3];
    uint8_t a_9;
    uint32_t b_9;
    uint32_t c_9[3];
    uint8_t a_10;
    uint32_t b_10;
    uint32_t c_10[3];
    uint8_t a_11;
    uint32_t b_11;
    uint32_t c_11[3];

};
#pragma pack()

format sss_format = {
    "1:#4:4-3",
    2,
    {"hello", "world", "bye"},
    // {"hello"}
};

format sss_format_2 = {
    "1:4:4-3:&1-7: 1:4:4-30: 1:4:4-3: 1:4:4-3: 1:4:4-3: 1:4:4-3: 1:4:4-3: 1:4:4-3 :1:4:4-3 :1:4:4-3 :1:4:4-3: 1:4:4-3",
    // 7,
    // 100,
    // 2,
    -1,
    {"hello", "world", "bye", "sdf", "erwer", "weorcxf", "weroicki324", "soiewrkjdfksdfj", "oiwerkldsfjklsjd", "oiwerj", NULL},
    // {"hello"}
};
struct st_data sss = {1,1234,{123456, 123, 23},
                        "bbbbbb",
                        1,1234,{123456, 123, 23,123456, 123, 23,123456, 123, 23,123456, 123, 23,123456, 123, 23,123456, 123, 23,123456, 123, 23,123456, 123, 23,123456, 123, 23,123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        1,1234,{123456, 123, 23},
                        };


int main(int argc, char *argv[])
{

    cJSON *root = NULL;
    #if 0
    root = format_to_json_simple(&sss, NULL, NULL);
    format_struct_to_json(&sss, &sss_format_2, "main", root);
    #else 

    root = format_to_json_simple(&sss, "main", NULL);
    int i;
        format_struct_to_json(&sss, &sss_format_2, NULL, root);
        char *out = cJSON_Print(root);
        print_jmt("out is %s", out);
        free(out);
    #endif

    cJSON_Delete(root);
}
#endif