#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <alloca.h>
#include "cJSON.h"
#include "cJSON_format.h"

#if 1 // test!!
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
#endif