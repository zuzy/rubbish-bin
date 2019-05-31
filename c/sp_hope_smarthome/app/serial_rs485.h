#ifndef __SERIAL_RS485_H__
#define __SERIAL_RS485_H__

#include "common.h"
#include "stdint.h"

#define HALO_RS485_DEBUG 0
#if HALO_RS485_DEBUG == 1
    #define print_serial(format, arg...)   do { printf("\033[31m[halo_serial]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_serial(format, arg...)   NULL
#endif

#define MAX_BODY_LEN 100

typedef struct _st_rs485_head
{
    uint8_t guid;
    uint8_t des_ad;
    uint8_t des_ch;
    uint8_t src_ad;
    uint8_t src_ch;
    uint8_t dev_type;
    uint8_t len;
} st_rs485_head;

typedef struct _st_rs485_body
{
    uint8_t cmd;
    uint8_t body[MAX_BODY_LEN];
}st_rs485_body;

typedef struct _st_rs485
{
    st_rs485_head   head;
    st_rs485_body   body;
    uint8_t         check;
}st_rs485;

int add_serial();

#endif