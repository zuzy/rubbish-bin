#ifndef __HALO_COMMON_H__
#define __HALO_COMMON_H__

#include <stdint.h>
#include <pthread.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "applib.h"
// #include <applib/psp_msg.h>
#include "list.h"
#include "cJSON.h"
#include <syslog.h>
#include "smart_player_api.h"

#define HALO_COMMON_DEBUG 1
#if HALO_COMMON_DEBUG == 1
    #define print_common(format, arg...)   do { printf("\033[31m[halo_common]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_common(format, arg...)   NULL
#endif

#define print_log(format, arg...)   do{ syslog(LOG_INFO, "[halotest log] %s,%d -> "format"\n",__FILE__,__LINE__, ## arg);}while(0)


#define REQUIRE(in,tag)                 do{if(in){print_log("%s %s", #tag, #in); goto tag;}}while(0)
#define REQUIRE_NOLOG(in,tag)           do{if(in){print_common("%s %s", #tag, #in); goto tag;}}while(0)
#define REQ_JSON_OBJ(des,item,tag)      cJSON_GetObjectItem(des,#item); \
                                        REQUIRE((item==NULL),tag)
#define REQ_JSON_PARSE(str,item,tag)    cJSON_Parse(str); \
                                        REQUIRE((item==NULL),tag)

#define MAX_BODY_LEN    1024
#define MAX_CONTENT_LEN 1024
#define MAX_PNAME_LEN   128
#define MAX_CMD_LEN     128
#define MAX_DID_LEN     32
#define MAX_ID_LEN      100

#define LOOP_CFG_HOPE       (1)
#define LOOP_CFG_ALINK      (1<<1)
#define LOOP_CFG_WIFI       (1<<2)
#define LOOP_CFG_UDP_SCAN   (1<<3)
#define LOOP_CFG_RETRY      (1<<4)

#define MAX_SUFFIX_LEN  6

// typedef struct
// {
//     /*! 消息类型 */
//     int  type;
//     /*! 消息内容 */
//     char content[MAX_CONTENT_LEN];
// }msg_apps_t;

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

typedef struct _st_pro_msg
{
    int     type;
    char    content[MAX_CONTENT_LEN];
    int     length;
}st_pro_msg;

typedef struct _st_msg
{
    struct list_head list;
    int     type;
    char    content[MAX_CONTENT_LEN];
    char    target[MAX_PNAME_LEN];
    int     length;
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

    uint32_t             config;

    uint64_t            timecount;
    uint64_t            timeout;
    int                 fd;
    st_buf              recv_buf;
    // st_hal              hal_cmd;
    splayer_status_t    play_status;

    st_callbacks        *callbacks;
    uint8_t             active;
    void                *private;
}st_proto;

typedef struct _st_halo
{
    pthread_t pid;
    struct list_head *protos;
    struct list_head *msgs;
    struct list_head *inits;
    callback    manager_cb;
    uint32_t     env_status;
    callback    hal_cb;

    void        *mlist;
    int         main_disk;
    int         music_num;
    
    char        auth[MAX_ID_LEN];
    char        ref_ori[MAX_ID_LEN];
    char        ref_hex[MAX_ID_LEN];
    char        ref_str[MAX_ID_LEN];
    int         *local_list;
    int         *list_index;
}st_halo;

typedef struct _st_dispatch
{
    char        cmd[MAX_CMD_LEN];
    callback    dispatch_cb;
}st_dispatch;

int add_proto(st_halo *halo, st_proto *proto);
int delete_proto(st_proto *proto);
int add_msg(struct list_head *head, char *target, st_pro_msg msg);
int del_msg(st_msg *msg);
int del_all_msg(struct list_head *msgs);
int clean_msg(st_halo *halo);
int close_proto_fd(int fd, struct list_head *head);
int close_proto(st_proto *proto);
void set_nonblocking(int fd);

uint64_t get_now_us(void);
uint64_t get_now_ms(void);
uint64_t get_now_s(void);

int reset_timer(st_proto *pro);
char *getip(void);

void clear_payload(st_proto *pro);

/** 
 * @brief  mv a buff forward steps
 * @note   mvforward a buff
 * @param  *des: the destination buff
 * @param  size: sizeof buff
 * @param  step: steps
 * @retval None
 */
void mvforward(char *des, int size, int step);

/** 
 * @brief  find and change the key to '\0'
 * @note   find the first key from string input; change it to '\0'; return the length forward
 * @param  *input: target string
 * @param  key: key char
 * @retval the length forward to the key
 */
int key_to_zero(char *input, char key);
// int str_to_zero(char *input, char *key);

/** 
 * @brief  traverse the buf, with '\n' ended; 
 * @note exp:  
 *       int tmp = 0; 
 *       FOREACH_LINE_OF_BUF(buf, length, tmp){ 
 *          printf("%s\n", buf);
 *          parse(buf);
 *          ...
 *      }
 *      will traverse and cut the buf with '\n' seperated
 * @retval null
 */
#define FOREACH_LINE_OF_BUF(buf, length, tmp)   for(; (tmp = key_to_zero(buf, '\n')) != 0; \
                                                    mvforward(buf, length, tmp), length -= tmp)
// #define FOREACH_LINE_OF_BUF(buf, length, tmp)   for(; (tmp = str_to_zero(buf, "\n")) != 0; \
//                                                     mvforward(buf, length, tmp), length -= tmp)
// #define FOREACH_LINE_OF_BUF(buf, length, tmp)   for(; (tmp = str_to_zero(buf, "\r\n")) != 0; \
//                                                     mvforward(buf, length, tmp), length -= tmp)


/** 
 * @brief  delete the sightless char ahead of the string
 * @note   just find the continuous sightless char ahead & moveforward the string
 * @param  *des: the target string
 * @param  len: length of string
 * @retval None
 */
void del_null(char *des, int len);
int change_flag(char *des, int size);
#if 0
void lua_call_func(const char *filename, 
                const char *func,
				const char *param,
                int len,
                char *ret);
#endif
/** 
 * @brief  dump all the stack of L
 * @note   ok the word 'stack' is spelled by mistake
 * @param  *L: the obj of lua
 * @retval None
 */
void lua_stachDump(lua_State *L);

/** 
 * @brief  common lua call function
 * @note   null
 * @param  *filename: target lua file
 * @param  *func: target lua function name
 * @param  *format: format of args 'xxx(input)>xxx(output)'
 *          i->int, d->double, s->char *;
 *       exp:   if ssid>isd : 
 *              char *input1, char *input2, int input3, double input4,
 *              int *output1, char *output2, double *output3;
 * @param  ...: args
 * @retval None
 */
void lua_call_fun(const char *filename,
                    const char *func,
                    const char *format,
                    ...);

/** 
 * @brief  http regist with hope cloud format!
 * @note   nil
 * @param  *uri: target uri 
 * @param  *body: body of post, just the 'dat' item of payload, other items are in lua.
 * @param  ret: the body return; 
 * @param  *code: post code return usually 200
 * @retval None
 */
void lua_regitst(const char *uri, 
                const char *body, 
                char* ret,
                int *code);
#endif

/** 
 * @brief  get the file name without suffix
 * @note   seperate the name
 * @param  *uri: des uri
 * @param  *name: target file name
 * @param  maxlen: max file name length
 * @retval 0->success
 */
int get_music_name(char *uri, char *name, int maxlen);

// 对应hope协议中歌曲列表情景指令
/** 
 * @brief  broadcast msg of playing local list
 * @note   null
 * @retval return status of broadcast
 */
int play_list_msg();

// 对应酒店版本, 扫描本地歌曲, 找到index开头的歌曲文件
/** 
 * @brief  seperate the uri to file name and suffix
 * @note   max_suffix_len default is 6
 * @param  *uri: target uri
 * @param  *name: target file name
 * @param  max_len: max length of file name
 * @param  *suffix: result suffix
 * @retval 0->success, -1 maybe without suffix
 */
int sep_file_suffix(char *uri, char *name, int max_len, char *suffix);

/** 
 * @brief  seperate the uri to index & suffix, 
 * @note   get the number ahead of the filename, get the suffix of file. To match the file %d_%s.media(mp3/wav...).
 * @param  *uri: des uri
 * @param  *suffix: result suffix
 * @retval -1->failed, maybe without suffix, 
 *          else -> result index of file;
 */
int sep_to_index_suffix(char *uri, char *suffix);

/**
 * @brief  format the uri of files in the ign menu
 * @retval 0
 */
int format_ign_list();

/**
 * @brief  demo of http request device list!
 * @note   
 * @retval None
 */
void lua_call_device();