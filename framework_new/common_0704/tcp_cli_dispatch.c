#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "common.h"
#include "cJSON.h"
#include "tcp_cli_dispatch.h"

struct _st_cli_msg
{
    st_cli_head head;
};


static int _seperate_buf(int len, char *buf, st_cli_head *head, char *body)
{
    len -= change_flag(buf, len);
    memcpy(head, buf, sizeof(st_cli_head));
    body = buf + sizeof(st_cli_head);
    return 0;
}

static int _common_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_cli("common reply!");
}

static int _heart_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
}

/** --------------------------------------
 * @brief  ctrl block
----------------------------------------*/

static int _cli_ctrl_status_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_play_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_control_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_skip_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_idvol_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_mute_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_setvol_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_source_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_effect_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_model_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_locale_cb(void *arg)
{
    int *value = (int *)arg;
    return 0;
Error:
    return -1;
}


#define CREATE_CTRL_UNIT(cmd)   {#cmd, _cli_ctrl_##cmd##_cb}
struct _st_cli_ctrl
{
    char        cmd[MAX_CMD_LEN];
    callback    ctrl_cb;
} cli_ctrl_handle[] = {
    CREATE_CTRL_UNIT(status),
    CREATE_CTRL_UNIT(play),
    CREATE_CTRL_UNIT(control),
    CREATE_CTRL_UNIT(skip),
    CREATE_CTRL_UNIT(idvol),
    CREATE_CTRL_UNIT(mute),
    CREATE_CTRL_UNIT(setvol),
    CREATE_CTRL_UNIT(source),
    CREATE_CTRL_UNIT(effect),
    CREATE_CTRL_UNIT(model),
    CREATE_CTRL_UNIT(locale),
    {"", NULL},
};

static int _ctrl_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    cJSON *root = REQ_JSON_PARSE(pro->private, root, Error);
    cJSON *profile = REQ_JSON_OBJ(root, profile, Error);
    struct _st_cli_ctrl *p = cli_ctrl_handle;
    while(*p->cmd) {
        cJSON *item = cJSON_GetObjectItem(profile, p->cmd);
        if(item != NULL) {
            int val = item->type == cJSON_Number ? item->valueint : item->type;
            print_cli("get cmd: %s -> %d", item->string, val);
            p->ctrl_cb(&val);
        }
    }
    return 0;
Error:
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    return -1;
}

static int _plist_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
}

static int _psongs_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
}

static int _info_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
}

struct _st_cli_dis {
    char name[32];
    int cmd;
    callback dis_cb;
} cli_dis[] = {
    {"common",  0x01,   _common_cb},
    {"heart",   0x02,   _heart_cb},
    {"ctrl",    0x05,   _ctrl_cb},
    {"p_list",  0x135,  _plist_cb},
    {"p_song",  0x140,  _psongs_cb},
    {"info",    0x205,  _info_cb},
    {"",        0,      NULL},
};

int cli_dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_cli_head head;
    _seperate_buf(pro->recv_buf.len, pro->recv_buf.payload, &head, pro->private);
    struct _st_cli_dis *ptr = cli_dis;
    head.cmd &= 0x0FFF;
    while(*ptr->name) {
        if(head.cmd == ptr->cmd) {
            ptr->dis_cb(pro); 
        }
    }
}