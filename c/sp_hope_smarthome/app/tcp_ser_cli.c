#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
// #include "net/config.h"
// #include "net/wifi_manager.h"
#include "list.h"
#include "common.h"
#include "main_loop.h"
#include "tcp_ser.h"
#include "tcp_ser_cli.h"
#include "tcp_ser_dispatch.h"
#include "cjson_list.h"
#include "hp_speech.h"
#include "sair_api.h"
#include "lua_common.h"

static int tsc_buf_len = 4096;
static int client_num = 0;

static int _dispatch(void *);


static int dispatch_lua(char *payload, char *body, int *len)
{
    int ret = -1;
    lua_call_fun("/tmp/lua/halo_parse.lua", "halo_parse", "s>isi", payload,&ret, body, len);
    print_ser_cli("ret %d %s %d", ret, body, *len);
    // switch(ret) {
    //     case 3: {
    //         print_ser_cli("search body!!!");
    //         sair_ai_text_search(body, strlen(body));
    //         break;
    //     }
    //     case 4: {
    //         print_ser_cli("tts body!!!");
    //         sair_ai_tts_call(body, strlen(body));
    //         break;
    //     }
    //     default: break;
    // }
    return ret;
}

int tcp_ser_cli_init(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_ser_cli("get a connect");
}

static int _debug_dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    if(pro->recv_buf.len > 200) {
        st_pro_msg msg;
        msg.type = 1;
        strcpy(msg.content, "hello!");
        msg.length = strlen(msg.content) + 1;
        add_msg(pro->msgs, "tcp_cli", msg);
        memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
        pro->recv_buf.len = 0;
    }
    return 0;
}

static int _get_songs_of_page(int num, char *cmd, char *buff)
{
    lua_call_fun("/tmp/lua/mlist.lua", "getpage", "is>s", num, cmd, buff);
    return 0;
}

static int _parse_playlist(char *str)
{
    cJSON *root = REQ_JSON_PARSE(str, root, Error);
    cJSON *total = REQ_JSON_OBJ(root, total, Error);
    cJSON *page = REQ_JSON_OBJ(root, page, Error);
    cJSON *cmd = REQ_JSON_OBJ(root, cmd, Error);
    int i = 1;
    char buff[MAX_BODY_LEN];
    for(; i <= page->valueint; i++) {
        memset(buff, 0, sizeof(buff));
        _get_songs_of_page(i, cmd->valuestring, buff);
        print_ser_cli("songs buf %s", buff);
        msg_to_ser_cli(0, buff);
    }
    if(root != NULL) {
        cJSON_Delete(root);
    }
    return 0;
    Error:{
        if(root != NULL) {
            cJSON_Delete(root);
        }
        return -1;
    }
}

int tcp_ser_cli_recv(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    int len = 0;
    char body[MAX_BODY_LEN] = "";
    int l = 0;
    len = recv(pro->fd, pro->recv_buf.payload + pro->recv_buf.len, pro->recv_buf.max_len, 0);
    if(len > 0) {
        pro->recv_buf.len += len;
        pro->recv_buf.payload[pro->recv_buf.len] = 0;
        print_ser_cli("recv: %d %s", pro->recv_buf.len, pro->recv_buf.payload);
        
        int tmp;
        FOREACH_LINE_OF_BUF(pro->recv_buf.payload, pro->recv_buf.len, tmp) {
            // if(dispatch_lua(pro->recv_buf.payload, body, &l) == 9) {
            //     return -1;
            // }
            #if 1
            /** 
             * @note   json list to parse body in case of get json together without 'LF'
             */
            struct list_head head = LIST_HEAD_INIT(head);
            get_json_list(&head, pro->recv_buf.payload);
            st_jsonlist *p;
            list_for_each_entry(p, &head, list) {
                char *out = cJSON_PrintUnformatted(p->item);
                print_ser_cli("get json list %s", out);
                memset(body, 0, sizeof(body));
                switch(dispatch_lua(out, body, &l)) {
                    case 1: {
                        msg_to_ser_cli(0, body);
                        break;
                    }
                    case 2: {
                        // music list
                        _parse_playlist(body);
                        break;
                    }
                    case 3: {
                        sair_ai_text_search(body, strlen(body));
                        break;
                    }
                    case 4: {
                        sair_ai_tts_call(body, strlen(body));
                        break;
                    }
                    case 9: {
                        free(out);
                        free_json_list(&head);
                        return -1;
                        break;
                    }
                    default: {
                        break;
                    }
                }
                free(out);
            }
            free_json_list(&head);
            #else
            switch(dispatch_lua(pro->recv_buf.payload, body, &l)) {
                case 1: {
                    msg_to_ser_cli(0, body);
                    break;
                }
                case 2: {
                    break;
                }
                case 3: {
                    sair_ai_text_search(body, strlen(body));
                    break;
                }
                case 4: {
                    sair_ai_tts_call(body, strlen(body));
                    break;
                }
                case 9: {
                    return -1;
                    break;
                }
                default: {
                    break;
                }
            }
            #endif
        }
        // if(_dispatch(pro) == 9) {
        //     return -1;
        // }
        // clear_payload(pro);
        reset_timer(pro);
        return 0;
    } else {
        if(errno == EINTR || 
            errno == EAGAIN ||
            errno == EWOULDBLOCK) {
            return 0;
        }
        return -1;
    }
}

int tcp_ser_cli_reply(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    // if(send(pro->fd, pro->recv_buf.payload, pro->recv_buf.len, 0) <= 0) {
    //     return -1;
    // }
    // _dispatch(pro);

    return 0;
}

/*
random,
single,
cycle
list
*/

struct _st_halo_mode {
    int mode;
    char name[32];
};

static struct _st_halo_mode mode[] = {
    {SPLAYER_LOOP_REPEAT_ALL, "cycle"},
    {SPLAYER_LOOP_REPEAT_ONE, "single"},
    {SPLAYER_LOOP_SHUFFLE, "random"},
    {SPLAYER_LOOP_SEQUENCE, "list"},
};

static struct _st_halo_mode effect[] = {
    {SPLAYER_EQ_CLASSIC, "classical"},
    {SPLAYER_EQ_POP, "modern"},
    {SPLAYER_EQ_DANCE, "dance"},
    {SPLAYER_EQ_HIPHOP, "pop"},
    {SPLAYER_EQ_NORMAL, "original"},
    {SPLAYER_EQ_ROCK, "rockroll"},
};

static struct _st_halo_mode playstate[] = {
    {SPLAYER_STATUS_PLAYING, "play"},
    {SPLAYER_STATUS_PAUSE, "pause"},
    {SPLAYER_STATUS_STOP, "stop"},
};

static struct _st_halo_mode source[] = {
    {SPLAYER_MODE_AIMUSIC, "alink"},
    {SPLAYER_MODE_CLOUDMUSIC, "speech"},
    {SPLAYER_MODE_BT, "bluetooth"},
    {SPLAYER_MODE_LOCAL, "local"},
    {SPLAYER_MODE_PLAYLIST, "local"},
    {SPLAYER_MODE_SAIR, "tts"},
    {SPLAYER_MODE_LINEIN, "linein"},
};

#define HALO_ADD_TO_JSON(json, model, n) do{ int i=0; int len = sizeof(model)/sizeof(mode[0]);for(;i<len;++i){ \
                                            if(n == model[i].mode){cJSON_AddStringToObject(json, #model, model[i].name);break;}}}while(0)

static char *_halo_status(splayer_status_t status)
{
    char *out = NULL;
    cJSON *root = NULL;
    cJSON *params = NULL;
    cJSON *current_song = NULL;
    REQUIRE((root = cJSON_CreateObject()) == NULL, Error);
    cJSON_AddStringToObject(root, "cmd", "info");
    REQUIRE((params = cJSON_CreateObject()) == NULL, Error);
    cJSON_AddItemToObject(root, "params", params);
    REQUIRE((current_song = cJSON_CreateObject()) == NULL, Error);
    cJSON_AddItemToObject(root, "currSong", current_song);
    HALO_ADD_TO_JSON(params, mode, status.loop_mode);
    HALO_ADD_TO_JSON(params, effect, status.eq_mode);
    HALO_ADD_TO_JSON(params, playstate, status.status);
    HALO_ADD_TO_JSON(params, source, status.mode);
    cJSON_AddNumberToObject(params, "volume", (status.volume * 3 + 2) >> 3);
    cJSON_AddNumberToObject(params, "process", status.current_time);
    cJSON_AddNumberToObject(params, "songid", status.playlist_index);

    splayer_info_t info;
    REQUIRE(splayer_get_info(&info) < 0, Error);
    if(*info.uri) {
        if(*info.album) {
            cJSON_AddStringToObject(current_song, "album", info.album);
        }
        if(*info.author) {
            cJSON_AddStringToObject(current_song, "artist", info.author);
        }
        if(strlen(info.title) > 0) {
            cJSON_AddStringToObject(current_song, "title", info.title);
            print_ser_cli("info title %s", info.title);
        } else {
            char t[128] = "";
            get_music_name(info.uri, t, sizeof(t) - 1);
            cJSON_AddStringToObject(current_song, "title", t);
            print_ser_cli("title is %s", t);
        }
        cJSON_AddNumberToObject(current_song, "duration", info.total_time);
        cJSON_AddNumberToObject(current_song, "id", status.playlist_index);
    }
    out = cJSON_PrintUnformatted(root);
    strcat(out, "\n");
    cJSON_Delete(root);
    return out;

    

    Error:{
        if(out != NULL) {
            free(out);
            out = NULL;
        }
        if(root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
    }

}

const char act_str[] = "hello halo";
int tcp_ser_cli_act(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    char *out = _halo_status(pro->play_status);
    print_ser_cli("act ! %s", out);
    int len = send(pro->fd, out, strlen(out), 0);
    free(out);
    if(len > 0)
        return 0;
    else 
        return -1;
}

static st_cmd halo_villa_cmd[] = {
    {SPEECH_CMD_DEV_INFO,   "getdevices"},
    {SPEECH_CMD_SCE_INFO,   "getscenes"},
    {SPEECH_CMD_RMS_INFO,   "getrooms"},
    {SPEECH_CMD_FLR_INFO,   "getfloors"},
    {SPEECH_CMD_SCE_CTRL,   "scenecontrol"},
    {SPEECH_CMD_DEV_CTRL,   "devicecontrol"},
    {SPEECH_CMD_EMD,        ""},
};

int msg_to_ser_cli(int type, char *content)
{
    st_pro_msg msg;
    strncpy(msg.content, content, sizeof(msg.content));
    msg.length = strlen(content) > sizeof(msg.content) ? sizeof(msg.content) : strlen(content);
    msg.type = type;
    return add_msg(halo_handle->msgs, "tcp_ser_cli", msg);
}

int tcp_ser_cli_dispro(void *arg, void *arg1)
{
    st_proto *pro = (st_proto *)arg;
    st_msg *msg = (st_msg *)arg1;
    print_ser_cli("msg to send %d->%s", msg->type, msg->content);
    if(strrchr(msg->content, '\n') == NULL) {
        strcat(msg->content, "\n");
    }
    #if 1
    REQUIRE(pro->fd < 0 || pro->active == 0, Exit);
    int len = strlen(msg->content);
    if(len > 0) {
        Retry:{
            if(len != send(pro->fd, msg->content, len, 0)) {
                REQUIRE((errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK), Retry);
                goto Exit;
            } else {
                print_ser_cli("send to %d success!", pro->fd);
                return 0;
            }
        }
    }
    Exit:{
        return -1;
    }
    #else
    cJSON *param = NULL;
    char *out = NULL;
    cJSON *root = cJSON_CreateObject();
    int ret = -1;
    REQUIRE((pro->fd < 0 || !pro->active), Exit);
    st_cmd *p = halo_villa_cmd;
    ret = 0;
    while(p->cmd < SPEECH_CMD_EMD && *p->content) {
        REQUIRE(p->cmd == msg->type, Add);
        ++p;
    }
    print_ser_cli("speech type is out of control %d", msg->type);
    goto Exit;
    Add: {
        cJSON_AddStringToObject(root, "cmd", p->content);
        param = REQ_JSON_PARSE(msg->content, param, Exit);
        cJSON_AddItemToObject(root, "params", param);
        REQUIRE((out = cJSON_PrintUnformatted(root)) == NULL, Exit);
        int len = strlen(out);
        Retry:{
            if(send(pro->fd, out, len, 0) != len) {
                if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                    goto Retry;
                }
                ret = -1;
            }
        }
    }
Exit: 
    NULL;
    if(out != NULL) {
        free(out);
        out = NULL;
    }
    if(root != NULL){
        cJSON_Delete(root);
        root = NULL;
    }
    return ret;
    #endif
}

int tcp_ser_cli_to(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    // ret != 0; manager judge timeout cb failed then close and clean the protocol obj!
    return -1;
}

int tcp_ser_cli_close(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    if(pro != NULL && pro->fd >= 0) {
        --client_num;
        close(pro->fd);
        pro->fd = -1;
        pro->active = 0;
        free(pro->recv_buf.payload);
        pro->recv_buf.len = 0;
        return 0;
    }
    return -1;
}

static st_callbacks ser_cli_cbs = {
    // tcp_ser_cli_init,
    NULL,
    tcp_ser_cli_recv,
    tcp_ser_cli_reply,
    tcp_ser_cli_act,
    tcp_ser_cli_dispro,
    tcp_ser_cli_to,
    tcp_ser_cli_close,
};

int add_tcp_ser_cli(int fd, char *name)
{
    st_proto *pro = (st_proto *)malloc(sizeof(st_proto));
    ++client_num;
    memset(pro, 0, sizeof(st_proto));
    if(name != NULL && *name != 0)
        strncpy(pro->name, name, MAX_PNAME_LEN);
    else
        strcpy(pro->name, "tcp_ser_cli");
    pro->fd = fd;
    socklen_t len = sizeof(unsigned int);
    int optval = 1;
    // setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, len);
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, len);
    pro->active = 1;
    pro->config |= LOOP_CFG_WIFI;
    pro->recv_buf.payload = (char *)malloc(tsc_buf_len);
    pro->recv_buf.max_len = tsc_buf_len;
    pro->callbacks = &ser_cli_cbs;
    pro->timeout = (45 << 10);
    pro->timecount = get_now_ms();
    print_ser_cli("get a client!");
    return add_proto(halo_handle, pro);
}

int tsc_active_num()
{
    return client_num;
}

static int _dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;

#if 1
    struct list_head head = LIST_HEAD_INIT(head);
    // pro->private = &head;
    
    get_json_list(&head, pro->recv_buf.payload);
    st_jsonlist *ptr, *tmp;
    list_for_each_entry_safe(ptr, tmp, &head, list) {
        pro->private = ptr->item;
        if(tcp_ser_dispatch(pro) == 9) {
            return 9;
        }
        pro->private = NULL;
    }
    free_json_list(&head);

#else
    cJSON *root = REQ_JSON_PARSE(pro->recv_buf.payload, root, Error);
    pro->private = root;
    // cJSON *cmd = REQ_JSON_OBJ(root, cmd, Error);
    tcp_ser_dispatch(pro);
    cJSON_Delete(root);
    root = NULL;
    clear_payload(pro);
#endif
    return 0;
Error:
    // if(root != NULL) {
    //     cJSON_Delete(root);
    //     root = NULL;
    // }
    return -1;
}

