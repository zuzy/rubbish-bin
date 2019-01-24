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
#include "tcp_client.h"
#include "halo_hal.h"
#include "main_loop.h"
#include "sair_api.h"

struct _st_cli_msg
{
    st_cli_head head;
};

static int _reply_(int cmd, char *str)
{
    char *buff = NULL;
    int size = 0;
    st_pro_msg msg;
    msg.type = C_DEV_TTS;
    REQUIRE(str == NULL, Error);
    REQUIRE((buff = _build_payload(C_DEV_TTS, str, &size)) == NULL, Error);
    
    memcpy(msg.content, buff, size);
    msg.length = size;
    add_msg(halo_handle->msgs, "tcp_cli", msg);
    free(buff);
    buff = NULL;
    free(str);
    str = NULL;
    return 0;
    Error: {
        if(str != NULL) {
            free(str);
            str = NULL;
        }
        if(buff != NULL) {
            free(buff);
            buff = NULL;
        }
        return -1;
    }
}

static void cli_dump(char *str, int len)
{
    int i;
    print_cli("get str!:");
    for(i = 0; i < len; i++) {
        printf(" %.02X", str[i]);
    }
    printf("\n");
}

static int _seperate_buf(int len, char *buf, st_cli_head *head, char *body)
{
    // REQUIRE(len <= 0, Error);
    if(len <= 0) goto Error;
    int length = 0;
    char *begin = buf;
    char *p = buf;
    do {
        --len;
    } while(*p++ != 0x7e);
    REQUIRE(len <= 0, Error);
    
    len -= change_flag(buf, len);
    REQUIRE(len <= 0, Error);
    
    memcpy(head, p, sizeof(st_cli_head));
    head->cmd = ntohs(head->cmd);
    head->len = ntohs(head->len);
    char *body_tmp = p + sizeof(st_cli_head);

    strcpy(body, body_tmp);

    while(*p != 0x7e) {
        ++p;
    }
    length = p - begin + 1;
    return length;
Error:
    return -1;
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
    return halo_play(*value);
Error:
    return -1;
}

static int _cli_ctrl_play_cb(void *arg)
{
    int *value = (int *)arg;
    return hope_music_play_index(*value);
Error:
    return -1;
}

static int _cli_ctrl_control_cb(void *arg)
{
    int *value = (int *)arg;
    return halo_cut(*value);
Error:
    return -1;
}

static int _cli_ctrl_skip_cb(void *arg)
{
    int *value = (int *)arg;
    return halo_seek(*value);
Error:
    return -1;
}

static int _cli_ctrl_idvol_cb(void *arg)
{
    int *value = (int *)arg;
    return halo_vol_ctrl(*value);
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_mute_cb(void *arg)
{
    int *value = (int *)arg;
    return halo_mute(*value);
    return 0;
Error:
    return -1;
}

static int _cli_ctrl_setvol_cb(void *arg)
{
    int *value = (int *)arg;
    return halo_vol(*value, 1);
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
    halo_loop_set(*value);
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
    if(check_is_alarm()) {
        print_cli("alarm ing");
        return 0;
    }
    st_proto *pro = (st_proto *)arg;
    cJSON *root = REQ_JSON_PARSE(pro->private, root, Error);
    cJSON *profile = REQ_JSON_OBJ(root, profile, Error);
    struct _st_cli_ctrl *p = cli_ctrl_handle;
    while(*p->cmd && p->ctrl_cb != NULL) {
        cJSON *item = cJSON_GetObjectItem(profile, p->cmd);
        if(item != NULL) {
            int val = item->type == cJSON_Number ? item->valueint : item->type;
            print_cli("get cmd: %s -> %d", item->string, val);
            p->ctrl_cb(&val);
        }
        ++p;
    }
    cJSON_Delete(root);
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
    if(check_is_alarm()) {
        print_cli("alarm ing");
        return 0;
    }
    st_proto *pro = (st_proto *)arg;
    print_cli("plist cb!");
    cJSON *root = REQ_JSON_PARSE(pro->private, root, Error);
    cJSON *musId = REQ_JSON_OBJ(root, musId, Error);
    print_cli("plist cb! %d", musId->valueint);
    int ret = hope_music_play_index(musId->valueint);
    print_cli("plist cb!");
    cJSON_Delete(root);
    root = NULL;
    return ret;
    // return halo_

    // cJSON *profile = REQ_JSON_OBJ(root, profile, Error);
    // struct _st_cli_ctrl *p = cli_ctrl_handle;
    // while(*p->cmd) {
    //     cJSON *item = cJSON_GetObjectItem(profile, p->cmd);
    //     if(item != NULL) {
    //         int val = item->type == cJSON_Number ? item->valueint : item->type;
    //         print_cli("get cmd: %s -> %d", item->string, val);
    //         p->ctrl_cb(&val);
    //     }
    //     ++p;
    // }
    // return 0;
Error:
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    return -1;

}

static int _psongs_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
}

static int _info_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    return hope_update_status(1);
    // return tcp_cli_update_status(pro);
}

static int* _sep_list_arr(char *str)
{
    char *p = NULL;
    int len = strlen(str);
    int *ret = NULL;
    if(len > 0) {
        // ret = (int *)malloc(sizeof(int) * ((len >> 1) + 1));       // "1,2"
        ret = calloc((len>>1) + 2, sizeof(int));
        if(ret == NULL) {
            return NULL;
        }
    } else {
        return NULL;
    }
    int *p_ret = ret;
    for(p = str; p - str <= len; ++p) {
        if(*p >= '0' && *p <= '9') {
            int num = 0;
            while((*p >= '0') && (*p <= '9')) {
                num *= 10;
                num += *p++ - '0';
            }
            *p_ret++ = num;
        }
    }
    *p_ret = -1;
    return ret;
}

static int _selist_cb(void *arg)
{
    if(check_is_alarm()) {
        print_cli("alarm ing");
        return 0;
    }
    st_proto *pro = (st_proto*)arg;
    cJSON *root = NULL;
    if(pro->private == NULL || strlen(pro->private) <= 0) {
        goto Error;
    }
    play_list_msg();
    // wait for other apps logout
    usleep(500000);
    root = REQ_JSON_PARSE(pro->private, root, Error);
    cJSON *musIds = REQ_JSON_OBJ(root, musIds, Error);
    if(halo_handle->local_list != NULL) {
        free(halo_handle->local_list);
        // halo_handle->local_list = halo_handle->list_index = NULL;
    }
    REQUIRE((halo_handle->local_list = _sep_list_arr(musIds->valuestring)) == NULL, Error);
    halo_handle->list_index = halo_handle->local_list;
    cJSON_Delete(root);
    root = NULL;
    halo_local_index(*halo_handle->list_index);
    return 0;
    
        

    Error:{
        if(root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        halo_handle->local_list = halo_handle->list_index = NULL;
        return -1;
    }
}

static int _tts_cmd_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    cJSON *root = NULL;
    cJSON *rep = NULL;
    char *out = NULL;
    int ret = 1;
    root = REQ_JSON_PARSE(pro->private, root, Error);
    cJSON *msgText = REQ_JSON_OBJ(root, msgText, Error);
    cJSON *msgCata = REQ_JSON_OBJ(root, msgCata, Error);
    switch(msgCata->valueint) {
        case 1: {
            ret = sair_ai_text_search(msgText->valuestring, strlen(msgText->valuestring));
            break;
        }
        case 2: {
            ret = sair_ai_tts_call(msgText->valuestring, strlen(msgText->valuestring));
            break;
        }
        default: {
            break;
        }
    }
    rep = cJSON_CreateObject();
    REQUIRE(rep == NULL, Error);
    cJSON_AddBoolToObject(rep, "status", ret);
    cJSON_AddNumberToObject(rep, "error", 5000);
    cJSON_AddStringToObject(rep, "receive", "");
    cJSON_AddNumberToObject(rep, "msgCata", msgCata->valueint);
    out = cJSON_PrintUnformatted(rep);

    REQUIRE(_reply_(C_DEV_TTS, out), Error);

    cJSON_Delete(rep);
    rep = NULL;
    cJSON_Delete(root);
    root = NULL;
    free(out);
    out = NULL;
    return 0;
    Error:{
        if(root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        if(out != NULL ) {
            free(out);
            out = NULL;
        }
        if(rep != NULL) {
            cJSON_Delete(rep);
            rep = NULL;
        }
        return -1;
    }
}

static int _tts_check_cb(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    cJSON *root = NULL;
    cJSON *rep = NULL;
    char *out = NULL;

    root = REQ_JSON_PARSE(pro->private, root, Error);
    cJSON *msgCata = REQ_JSON_OBJ(root, msgCata, Error);

    REQUIRE((rep = cJSON_CreateObject()) == NULL, Error);
    splayer_info_t info;
    splayer_get_info(&info);
    if(*info.title) {
        cJSON_AddStringToObject(rep, "msgText", info.title);
    } else {
        char title[256] = "";
        get_music_name(info.uri, title, sizeof(title)-1);
        cJSON_AddStringToObject(rep, "msgText", title);
    }
    cJSON_AddBoolToObject(rep, "status", 1);
    cJSON_AddNumberToObject(rep, "error", 5000);
    cJSON_AddStringToObject(rep, "receive", "");
    cJSON_AddNumberToObject(rep, "msgCata", 1);
    out = cJSON_PrintUnformatted(rep);
    REQUIRE(_reply_(C_DEV_TTS_CHECK, out), Error);
    cJSON_Delete(rep);
    cJSON_Delete(root);
    free(out);
    out = root = rep = NULL;
    return 0;
    Error:{
        if(rep != NULL) {
            cJSON_Delete(rep);
        }
        if(root != NULL) {
            cJSON_Delete(root);
        }
        if(out != NULL) {
            free(out);
        }
        out = root = rep = NULL;
        return -1;
    }
}

struct _st_cli_dis {
    char name[32];
    int cmd;
    callback dis_cb;
} cli_dis[] = {
    {"common",      0x01,       _common_cb},
    {"heart",       0x02,       _heart_cb},
    {"ctrl",        0x05,       _ctrl_cb},
    {"p_list",      0x135,      _plist_cb},
    {"p_song",      0x140,      _psongs_cb},
    {"info",        0x205,      _info_cb},
    {"sel_list",    0x235,      _selist_cb},
    {"tts_cmd",     0x245,      _tts_cmd_cb},
    {"tts_check",   0x255,      _tts_check_cb},
    {"",            0,          NULL},
};

int cli_dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    st_cli_head head;
    int len = 0;
    #if 1
    if(check_is_alarm()) {
        print_cli("alarm ing");
        clear_payload(pro);
        return 0;
    }
    #endif
    if(pro->private != NULL) {
        free(pro->private);
    }
    pro->private = calloc(1, 1024);
    // pro->private = malloc(1024);
    while(1) {
        // memset(pro->private, 0, 1024);
        len = _seperate_buf(pro->recv_buf.len, pro->recv_buf.payload, &head, pro->private);
        if(len > pro->recv_buf.len || len < 0) {
            break;
        } 
        struct _st_cli_dis *ptr = cli_dis;
        head.cmd &= 0x0FFF;
        print_cli("get command %X %s", head.cmd, pro->private);
        while(*ptr->name && ptr->dis_cb != NULL) {
            if(head.cmd == ptr->cmd) {
                ptr->dis_cb(pro); 
                break;
            }
            ++ptr;
        }
        mvforward(pro->recv_buf.payload, pro->recv_buf.len, len);
        pro->recv_buf.len -= len;
    }
    if(pro->private != NULL) {
        free(pro->private);
        pro->private = NULL;
    }
    return 1;
}