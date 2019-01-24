#ifndef __HALO_COMMON_H__
#define __HALO_COMMON_H__

#include <stdint.h>
#include <pthread.h>
#include "list.h"
#include "cJSON.h"

#define HALO_COMMON_DEBUG 1
#if HALO_COMMON_DEBUG == 1
    #define print_common(format, arg...)   do { printf("\033[31m[halo_common]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_common(format, arg...)   do {} while (0)
#endif

#define REQUIRE(in,tag)                 do{if(in){print_common("err! %s", #in); goto tag;}}while(0)
#define REQ_JSON_OBJ(des,item,tag)      cJSON_GetObjectItem(des,#item); \
                                        REQUIRE((item==NULL),tag)
#define REQ_JSON_PARSE(str,item,tag)    cJSON_Parse(str); \
                                        REQUIRE((item==NULL),tag)

#define MAX_BODY_LEN    1024
#define MAX_CONTENT_LEN 1024
#define MAX_PNAME_LEN   128
#define MAX_CMD_LEN     128
#define MAX_DID_LEN     32

#define LOOP_CFG_WIFI   (1)
#define LOOP_CFG_ALINK  (1<<1)
#define LOOP_CFG_REGIST (1<<2)
#define LOOP_CFG_RETRY  (1<<3)

typedef struct
{
    /*! 消息类型 */
    int  type;
    /*! 消息内容 */
    char content[MAX_CONTENT_LEN];
}msg_apps_t;

typedef struct _st_buf
{
    char    *payload;
    int     max_len;
    int     len;
    int     begin;
}st_buf;

typedef struct _st_cmd
{
    int     cmd;
    char    content[MAX_CMD_LEN];
}st_cmd;

typedef struct _st_hal
{
    uint16_t    enable;
    uint8_t     play_ctrl;
    uint8_t     vol_ctrl;
    uint32_t    seek_ctrl;
    // uint32_t    total_time;
    uint8_t     loop_mode;
    uint8_t     eq_mode;
    uint8_t     play_mode;
    uint8_t     nop;
}st_hal;

typedef struct _st_msg
{
    struct list_head list;
    int     type;
    char    content[MAX_CONTENT_LEN];
    char    target[MAX_PNAME_LEN];
}st_msg;

typedef int (*callback)(void *);
typedef int (*dispro_callback)(void *, void *);

typedef struct _st_callbacks{
    callback            init_cb;
    callback            recv_cb;
    callback            reply_cb;
    callback            active_send_cb;
    dispro_callback     disproto_cb;
    callback            timeout_cb;
    callback            close_cb;
}st_callbacks;

typedef struct _st_proto
{
    struct list_head    list;
    struct list_head    *msgs;
    char                name[MAX_PNAME_LEN];

    uint8_t             config;
    uint8_t             active;

    int                 timecount;
    int                 timeout;
    int                 fd;
    st_buf              recv_buf;
    st_hal              hal_cmd;

    st_callbacks        *callbacks;
    void                *private;
}st_proto;

typedef struct _st_halo
{
    pthread_t pid;
    struct list_head *protos;
    struct list_head *msgs;
    callback    manager_cb;
    uint8_t     env_status;
    callback    hal_cb;
}st_halo;

typedef struct _st_dispatch
{
    char        cmd[MAX_CMD_LEN];
    callback    dispatch_cb;
}st_dispatch;

int add_proto(st_halo *halo, st_proto *proto);
int delete_proto(st_proto *proto);
int add_msg(struct list_head *head, char *target, msg_apps_t msg);
int del_msg(st_msg *msg);
int del_all_msg(struct list_head *msgs);
int clean_msg(st_halo *halo);
int close_proto_fd(int fd, struct list_head *head);
int close_proto(st_proto *proto);
void set_nonblocking(int fd);

int64_t get_now_us(void);
int64_t get_now_s(void);
int reset_timer(st_proto *pro);
char *getip(void);

void clear_payload(st_proto *pro);

void mvforward(char *des, int size, int step);
void del_null(char *des, int len);
int change_flag(char *des, int size);

#endif

