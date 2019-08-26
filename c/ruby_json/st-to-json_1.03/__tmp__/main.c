#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <alloca.h>
#include "cJSON.h"
#include "cJSON_format.h"


// #define _NODE_(T, N, E)
// #define PAYLOAD \
//     _NODE_(char*, name, P_STR) \
//     _NODE_(int, value, NUMBER) \
//     _NODE_(char, payload[34], A_STR)
// #undef _NODE_



#define _NODE_(T, N, E)
#define _NODE_ARRAY(T, S, N, E)
#define PAYLOAD \
    _NODE_(int, value, NUMBER) \
    _NODE_(int, value1, NUMBER) \
    _NODE_(int, value2, NUMBER) \
    _NODE_(int, value3, NUMBER) \
    _NODE_(float, value4, FLOAT) \
    _NODE_(double, value5, DOUBLE) \
    _NODE_(char *, value6, P_STR) \
    _NODE_(char [64], value7, A_STR) \
    _NODE_(int *, value8, P_INT) \
    _NODE_(float *, value9, P_FLOAT) \
    _NODE_(double *, value10, P_DOUBLE) \
    _NODE_ARRAY(int[11], int, value11, NUMBER) \
    _NODE_ARRAY(double[11], double, value12, DOUBLE) \
    _NODE_ARRAY(char *[12], char *, value13, P_STR) \
    _NODE_ARRAY(char [12][32], char[32], value14, A_STR) 
#undef _NODE_ARRAY
#undef _NODE_


typedef struct test {
    #define _NODE_(T, N, E) __typeof__(T) N;
    #define _NODE_ARRAY(T, S, N, E) __typeof__(T) N;
    PAYLOAD
    #undef _NODE_ARRAY
    #undef _NODE_
}test_t;

json_fmt_t fmt_test[] = {
    // #define _TARGET_    __typeof__(struct test)
    #define _NODE_(T,N,E) {.name=#N,.offset=offsetof(struct test, N), .size=sizeof(T), .size_node=0, .type=E},
    #define _NODE_ARRAY(T, S, N, E) {.name=#N,.offset=offsetof(struct test,N), .size=sizeof(T), .size_node=sizeof(S), .type=E},
    PAYLOAD
    #undef _NODE_ARRAY
    #undef _NODE_
    // #undef _TARGET_ 
};


#define _NODE_(T, N, E)
#define _NODE_ARRAY(T, S, N, E) 
#define _NODE_STRUCT(T, N, E, F)

#define PAYLOAD_LARGE \
    _NODE_(int, value, NUMBER) \
    _NODE_(int, value1, NUMBER) \
    _NODE_(int, value2, NUMBER) \
    _NODE_(int, value3, NUMBER) \
    _NODE_STRUCT(test_t, value4, STRUCT, fmt_test)

#undef _NODE_STRUCT
#undef _NODE_ARRAY
#undef _NODE_


struct large {
    #define _NODE_(T, N, E) __typeof__(T) N;
    #define _NODE_ARRAY(T, S, N, E) __typeof__(T) N;
    #define _NODE_STRUCT(T, N, E, F) __typeof__(T) N;

    PAYLOAD_LARGE

    // #define PAYLOAD_LARGE \
    //     _NODE_(int, value, NUMBER) \
    //     _NODE_(int, value1, NUMBER) \
    //     _NODE_(int, value2, NUMBER) \
    //     _NODE_(int, value3, NUMBER) \
    //     _NODE_STRUCT(test_t, vvvv, STRUCT, fmt_test, sizeof(fmt_test) / sizeof(fmt_test[0]))

    #undef _NODE_STRUCT
    #undef _NODE_ARRAY
    #undef _NODE_
};

json_fmt_t fmt[] = {
    #define _TARGET_    __typeof__(struct large)
    #define _NODE_(T,N,E) {.name=#N,.offset=offsetof(struct large, N), .size=sizeof(T), .size_node=0, .type=E},
    #define _NODE_ARRAY(T, S, N, E) {.name=#N,.offset=offsetof(struct large,N), .size=sizeof(T), .size_node=sizeof(S), .type=E},
    #define _NODE_STRUCT(T, N, E, F) {.name=#N,.offset=offsetof(struct large, N), .size=sizeof(T), .size_node=0, .type=E, .fmt=&F, .fmt_size=sizeof(F) / sizeof(json_fmt_t)},
    
    PAYLOAD_LARGE

    #undef _NODE_STRUCT
    #undef _NODE_ARRAY
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
        .value11 = {1,3,4,5,6},
        .value12 = {12,32,45,657,1},
        .value13 = {"123", "hello","bye"},
        .value14 = {"b bvbb", "1234", "sddsd"},
    };
    struct large larg = {
        1,2,
        .value4={
            1,2,3,4,5.1,6.2,
            .value6 = "123456",
            .value7 = "bye",
            .value11 = {1,3,4,5,6},
            .value12 = {12,32,45,657,1},
            .value13 = {"123", "hello","bye"},
            .value14 = {"b bvbb", "1234", "sddsd"},
        },
    };

    print_jmt("%p, %p %p -> %s",&t, &t.value6, t.value6, t.value6);
    char *tmp = t.value6;
    int value8 = 10;
    float value9 = 11;
    double value10 = 12;
    // larg.value4.value8 = t.value8 = &value8;
    // larg.value4.value9 = t.value9 = &value9;
    // larg.value4.value10 = t.value10 = &value10;

    // print_jmt("tmp is %p %s", tmp, tmp);
    
    // print_jmt("%p, %p %p -> %d",&t, &t.value8, t.value8, *t.value8);
    cJSON *root = struct_to_json(&larg, fmt, sizeof(fmt) / sizeof(fmt[0]), "sss", NULL);
    char *out = cJSON_Print(root);
    cJSON_Delete(root);
    print_jmt("root is %s\n", out);
    free(out);

    root = struct_to_json(&t, fmt_test, sizeof(fmt_test) / sizeof(fmt_test[0]), NULL, NULL);

    out = cJSON_Print(root);
    print_jmt("test is %s\n", out);
    cJSON_Delete(root);
    free(out);
    return 0;
}

