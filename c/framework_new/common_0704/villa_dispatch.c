#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "cJSON.h"
#include "villa.h"
#include "villa_dispatch.h"

static int _heartbeat(void *);
static int _device_list(void *);
static int _scene_list(void *);
static int _room_list(void *);
static int _floor_list(void *);
static int _attribute_list(void *);
static int _ctrl(void *);
static int _m_ctrl(void *);
static int _m_specify(void *);
static int _m_tunnel(void *);
static int _m_info(void *);
static int _m_list(void *);

static struct _st_villa_pro _villa_dis_handle[] = {
    {VILLA_HEART_RE,    RE_HEART,      _heartbeat},
    {VILLA_DLIST_RE,    RE_DEV,        _device_list},
    {VILLA_SLIST_RE,    RE_SCE,        _scene_list},
    {VILLA_RLIST_RE,    RE_RM,         _room_list},
    {VILLA_FLIST_RE,    RE_FLR,        _floor_list},
    {VILLA_ALIST_RE,    RE_ATTR,       _attribute_list},
    {VILLA_CTRL_RE,     RE_CTL,        _ctrl},
    {VILLA_ASSIST,      RE_M_CTRL,     _m_ctrl},
    {VILLA_ASSIST,      RE_M_SPE,      _m_specify},
    {VILLA_ASSIST,      RE_M_TUN,      _m_tunnel},
    {VILLA_ASSIST,      RE_M_INFO,     _m_info},
    {VILLA_ALIST,       RE_M_LIST,     _m_list},

    {VILLA_NOP,         "",             NULL}
};

static int _heartbeat(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _device_list(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _scene_list(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _room_list(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _floor_list(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _attribute_list(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _ctrl(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _m_ctrl(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _m_specify(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _m_tunnel(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _m_info(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}
static int _m_list(void *arg)
{
    print_villa("dispatch %s", __FUNCTION__);
    return 0;
Error:
    NULL;
    return 1;
}

int villa_parse(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    uint8_t chk = 0;
    int i, len;
    uint8_t *ptr;
    struct _st_villa_pro *p_handle = _villa_dis_handle;
    cJSON *root = NULL;

    memcpy(&villa_data, pro->recv_buf.payload, pro->recv_buf.len);
    villa_data.length = ntohs(villa_data.length);
    villa_data.cmd = ntohs(villa_data.cmd);
    villa_data.chk = villa_data.data[villa_data.length];

    print_villa("recv %d %.02X chk %.02X", villa_data.length, villa_data.cmd, villa_data.chk);
    len = villa_data.length + 4;
    ptr = (uint8_t *)&villa_data;
    ++ptr;
    for(i = 0; i < len; i++) {
        chk ^= *ptr++;
    }
    print_villa("chk cal (%.02X, %.02X)", chk, villa_data.chk);
    print_villa("get body %s", villa_data.data);
    REQUIRE(chk != villa_data.chk, Error);
    
    root = REQ_JSON_PARSE((char *)villa_data.data, root, Error);
    cJSON *type = REQ_JSON_OBJ(root, type, Error);
    print_villa("type : %s", type->valuestring);
    while(p_handle->discb != NULL) {
        print_villa("p_handle name is %s", p_handle->name);
        if(p_handle->cmd == villa_data.cmd) {
            print_villa("get common fix %.02X", p_handle->cmd);
            if(strcmp(type->valuestring, p_handle->name) == 0) {
                print_villa("get type ok %s", type->valuestring);
                p_handle->discb(NULL);
                break;
            }
        }
        ++p_handle;
    }
    cJSON_Delete(root);
    return 0;
Error:
    NULL;
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    return -1;
}