#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "tcp_ser_dispatch.h"
#include "halo_hal.h"

int tcp_ser_info_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_ser("info!");
    return 0;
Error:
    return -1;
}

#define CREATE_CTRL_UNIT(cmd)    {#cmd,_ctrl_##cmd##_cb}

#define EFFECT_NUM 6
static char *effect_str[EFFECT_NUM] = {
    "classical",
    "modern",
    "rockroll",
    "pop",
    "dance",
    "original",
};

static int _ctrl_effect_cb(void *arg)
{
    cJSON *item = (cJSON *)arg;
    int i = 0;
    for(i = 0; i < EFFECT_NUM; ++i) {
        if(strcmp(effect_str[i], item->valuestring) == 0) {
            // set_effect_mode(i);
            print_ser("cmd %s : %s", item->string, item->valuestring);
            break;
        }
    }
    return 0;
}

#define SOURCE_NUM  3
static char *source_str[SOURCE_NUM] = {
    "local",
    "bluetooth",
    "linein",
};

static int _ctrl_source_cb(void *arg)
{
    cJSON *item = (cJSON *)arg;
    int i = 0;
    for(i = 0; i < SOURCE_NUM; ++i) {
        if(strcmp(source_str[i], item->valuestring) == 0) {
            // set source(i);
            print_ser("cmd %s : %s", item->string, item->valuestring);
            break;
        }
    }
    return 0;
}


static int _inc_vol(void *arg)
{
    return halo_vol_ctrl(1);
}

static int _dec_vol(void *arg)
{
    return halo_vol_ctrl(0);
}

static int _mute_vol(void *arg)
{
    return halo_mute(1);
}

static int _demute_vol(void *arg)
{
    return halo_mute(0);
}

static struct _st_vol_ctrl
{
    char name[7];
    callback vol_cb;
} ser_vol[] = {
    {"inc", _inc_vol},
    {"dec", _dec_vol},
    {"mute", _mute_vol},
    {"unmute", _demute_vol},
    {"", NULL}
};

static int _ctrl_volume_cb(void *arg)
{
    cJSON *item = (cJSON *)arg;
    if(item == NULL) 
        return -1;
    switch(item->type) {
        case cJSON_String: {
            struct _st_vol_ctrl *p = ser_vol;
            while(p->vol_cb != NULL) {
                if(strcmp(p->name, item->valuestring) == 0) {
                    return p->vol_cb(NULL);
                }
                ++p;
            }
            int vol = atoi(item->valuestring);
            print_ser("cmd %s : %s %d", item->string, item->valuestring, vol);
            return halo_vol((vol << 3) / 3, 0);
            // return -1;
        }
        case cJSON_Number:
        default: break;
    }
    return -1;
}

#define MODE_NUM 5
static char *mode_str[MODE_NUM] = {
    "null",
    "random",
    "cycle",
    "single",
    "list",
};

static int _ctrl_mode_cb(void *arg)
{
    cJSON *item = (cJSON *)arg;
    int i = 0;
    for(; i < MODE_NUM; i++) {
        if(strcmp(mode_str[i], item->valuestring) == 0){
            print_ser("cmd %s : %s", item->string, item->valuestring);
            return halo_loop_set(i);
        }
    }
    return -1;
}

#define PLAYSTATE_NUM 4
static char *playstate_str[PLAYSTATE_NUM] = {
    "pause",
    "play",
    "prev",
    "next",
};

static int _ctrl_playstate_cb(void *arg)
{
    cJSON *item = (cJSON *)arg;
    int i = 0;
    for(; i < PLAYSTATE_NUM; i++) {
        if(strcmp(playstate_str[i], item->valuestring) == 0) {
            print_ser("cmd %s : %s %d", item->string, item->valuestring, i);
            if(i < 2) {
                return halo_play(i);
            } else {
                return halo_cut(i - 2);
            }
        }
    }
    return -1;
}

static int _ctrl_progress_cb(void *arg)
{
    cJSON *item = (cJSON *)arg;
    int value = -1;
    if(item->type == cJSON_String) {
        value = atoi(item->valuestring);
    } else {
        value = item->valueint;
    }
    if(value >= 0){
        print_ser("%s -> %d", item->string, value);
        return halo_seek(value);
    }
    return -1;
}

static int _ctrl_songid_cb(void *arg)
{
    cJSON *item = (cJSON *)arg;
    int value = -1;
    if(item->type == cJSON_String) {
        if(*item->valuestring <= '9' && *item->valuestring >= '0')
            value = atoi(item->valuestring);
    } else {
        value = item->valueint;
    }
    if(value >= 0) {
        print_ser("%s -> %d", item->string, value);
        return hope_music_play_index(value);
    }
    return -1;
}

static int _ctrl_currsong_cb(void *arg)
{
    cJSON *item = (cJSON *)arg;
    return 0;
}

static st_dispatch ctrl[] = {
    CREATE_CTRL_UNIT(effect),
    CREATE_CTRL_UNIT(source),
    CREATE_CTRL_UNIT(volume),
    CREATE_CTRL_UNIT(mode),
    CREATE_CTRL_UNIT(playstate),
    CREATE_CTRL_UNIT(progress),
    CREATE_CTRL_UNIT(songid),
    CREATE_CTRL_UNIT(currsong),
    {"", NULL},
};

int tcp_ser_control_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_ser("control");

    cJSON *root = (cJSON *)pro->private;
    cJSON *params = REQ_JSON_OBJ(root, params, Error);
    st_dispatch *ptr = ctrl;
    while(ptr->dispatch_cb != NULL) {
        cJSON *item = cJSON_GetObjectItem(params, ptr->cmd);
        if(item != NULL) {
            ptr->dispatch_cb(item);
        }
        ++ptr;
    }
    return 0;
Error:
    return -1;
}
int tcp_ser_getsonglist_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_ser("get song list!");
    return 0;
Error:
    return -1;
}
int tcp_ser_getplaylist_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_ser("get play list!");
    return 0;
Error:
    return -1;
}
int tcp_ser_querysonginfo_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;

    return 0;
Error:
    return -1;
}
int tcp_ser_operlist_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;

    return 0;
Error:
    return -1;
}
int tcp_ser_exit_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_ser("exit!!!");
    // just as same as kill -9?
    // close the connect by force
    return 9;
Error:
    return -1;
}
int tcp_ser_opertunnel_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;

    return 0;
Error:
    return -1;
}
int tcp_ser_deviceinfo_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_ser("dev info");
    return 0;
Error:
    return -1;
}
int tcp_ser_hopediscover_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;

    return 0;
Error:
    return -1;
}
#define CREATE_DIS_UNIT(cmd)    {#cmd,tcp_ser_##cmd##_cb}
st_dispatch tcp_ser_dis[] = {
    CREATE_DIS_UNIT(info),
    CREATE_DIS_UNIT(control),
    CREATE_DIS_UNIT(getsonglist),
    CREATE_DIS_UNIT(getplaylist),
    CREATE_DIS_UNIT(querysonginfo),
    CREATE_DIS_UNIT(operlist),
    CREATE_DIS_UNIT(exit),
    CREATE_DIS_UNIT(opertunnel),
    CREATE_DIS_UNIT(deviceinfo),
    CREATE_DIS_UNIT(hopediscover),
    {"", NULL},
};

int tcp_ser_dispatch(st_proto *pro)
{
    cJSON *root = NULL;
    REQUIRE((pro == NULL), Error);
    root = (cJSON *)pro->private;
    REQUIRE((root == NULL), Error);
    st_dispatch *ptr = tcp_ser_dis;
    cJSON *cmd = REQ_JSON_OBJ(root, cmd, Error);
    while(*ptr->cmd) {
        if(strncmp(ptr->cmd, cmd->valuestring, MAX_CMD_LEN) == 0) {
            print_ser("find cmd %s then dispatch", cmd->valuestring);
            return ptr->dispatch_cb(pro);
        }
        ++ptr;
    }
    print_ser("cannot find cmd %s", cmd->valuestring);
Error:
    return -1;    
}