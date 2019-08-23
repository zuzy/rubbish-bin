#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <alloca.h>
#include "cJSON.h"
#include "cJSON_format.h"

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
                        "bbbb\r\n",
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
#else

// #define _NODE_(T, N, E)
// #define PAYLOAD \
//     _NODE_(char*, name, P_STR) \
//     _NODE_(int, value, NUMBER) \
//     _NODE_(char, payload[34], A_STR)
// #undef _NODE_



#define _NODE_(T, N, E)
#define PAYLOAD \
    _NODE_(int, value, NUMBER) \
    _NODE_(int, value1, NUMBER) \
    _NODE_(int, value2, NUMBER) \
    _NODE_(int, value3, NUMBER) \
    _NODE_(float, value4, FLOAT) \
    _NODE_(double, value5, DOUBLE) \
    _NODE_(char *, value6, P_STR) \
    _NODE_(char [64], value7, A_STR) \
#undef _NODE_

struct test {
    #define _NODE_(T, N, E) __typeof__(T) N;
    PAYLOAD
    #undef _NODE_
};

json_fmt_t fmt[] = {
    // #define _TARGET_    __typeof__(struct test)
    #define _NODE_(T,N,E) {.name=#N,.offset=offsetof(struct test, N), .size=sizeof(T), .type=E},
    PAYLOAD
    #undef _NODE_
    // #undef _TARGET_ 
};


char *get_name(const char *target) 
{
    if(!target) return NULL;
    char *return_str = calloc(1,strlen(target) + 1);
    if(!return_str) return NULL;
    char *pr = return_str, *pt = target;
    while(*pt) {
        if(*pt >= ' ' && *pt <= '`') {
            *pr++ = *pt;
        }
        ++pt;
    }
    return pr;
}




#ifndef container_of
#define container_of(ptr, type, member) 			\
        (type *)( (char *)(ptr) - offsetof(type, member) )
#endif

int main(int argc, char *argv[])
{
    struct test t = {
        1,2,3,4,5.1,6.2,
        .value6 = "123456",
        .value7 = "bye",
    };

    print_jmt("%p, %p %p -> %s",&t, &t.value6, t.value6, t.value6);
    char *tmp = t.value6;
    print_jmt("tmp is %p %s", tmp, tmp);
    

    cJSON *root = struct_to_json(&t, fmt, sizeof(fmt) / sizeof(fmt[0]), "sss", NULL);
    char *out = cJSON_Print(root);
    print_jmt("root is %s\n", out);
    free(out);
    return 0;
}

#endif