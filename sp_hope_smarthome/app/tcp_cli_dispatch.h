#ifndef __TCP_CLI_DISPATCH_H__
#define __TCP_CLI_DISPATCH_H__

#include <stdint.h>
#include "list.h"
#include "cJSON.h"
#include "cjson_list.h"
#include "tcp_client.h"

#pragma pack(1)
typedef struct _st_cli_head
{
    uint16_t cmd;
    uint8_t   type;
    uint16_t  len;
    uint8_t   snd_id[10];
}st_cli_head;
#pragma pack()

int cli_dispatch(void *arg);
#endif