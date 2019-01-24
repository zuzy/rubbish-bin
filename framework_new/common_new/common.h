#ifndef __HALO_COMMON_H__
#define __HALO_COMMON_H__

#include <stdint.h>
#include "list.h"

#define HALO_COMMON_DEBUG 1
#if HALO_COMMON_DEBUG == 1
    #define print_common(format, arg...)   do { printf("\033[31m[halo_common]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_common(format, arg...)   do {} while (0)
#endif

#define MAX_BODY_LEN    1024
#define LOOP_CFG_WIFI   (1)
#define LOOP_CFG_ALINK  (1<<1)

typedef int (*init_callback)(void *);
typedef int (*close_callback)(int);
typedef int (*loop_init_callbck)(void);
typedef int (* loop_callback)(void *);
typedef int (*send_callback)(int fd, void *arg);

typedef struct _st_cmd
{
    int     cmd;
    char    content[16];
}st_cmd;

typedef int (*halo_hal_callback)(st_cmd cmd);

typedef enum _enum_error
{
    ERR_IGN = 0,
    ERR_CLOSE,
    ERR_FORCE_LOCALE,
    ERR_FORCE_INIT,
    ERR_FORCE_REBOOT,
}enum_error;

typedef enum_error (*common_callback)(void);


typedef struct _st_init
{
    struct list_head    list;
    init_callback       callback;
    void                *arg;
}st_init;

typedef struct _st_body
{
    char    *payload;
    size_t  max_len;
    int     len;
    char    *p_body;
    int     is_start;
}st_body;


typedef struct _st_loop_init
{
    struct list_head    list;
    int                 fd;
    loop_init_callbck   loop_init_cb;
    loop_callback       loop_cb;
    close_callback      close_cb;
    int                 len;
    uint8_t             cfg;
}st_loop_init;

typedef struct _st_loop
{
    struct list_head    list;
    int                 fd;
    loop_callback       callback;
    st_body             body;
    st_cmd              cmd;
    close_callback      close_cb;
    uint8_t             cfg;
}st_loop;


typedef struct _st_send
{
    struct list_head    list;
    int                 fd;
    send_callback       send_cb;
    st_body             body;
}st_send;


typedef struct _st_loop_new
{
    struct list_head    *init_list;
    struct list_head    *loop_list;
    struct list_head    *send_list;
    int                 update;
    halo_hal_callback   hal_callback;
}st_loop_new;


int halo_init_add(init_callback callback, void *arg);
int halo_send_add(int fd, send_callback callback, int len);
int halo_send_del(st_send *send);
int halo_loop_init_add(loop_init_callbck init_cb,
                loop_callback loop_cb, 
                close_callback close_cb,
                int len,
                uint8_t cfg);
int halo_loop_init_del(st_loop_init *init);
int halo_loop_del(st_loop *loop);
int halo_common_del(int fd);
int halo_common_send(st_cmd cmd);
int halo_common_init();
int halo_common_deinit();
int halo_common_start();
int halo_common_stop();
int halo_update(st_cmd cmd);

int halo_loop_recv_add(int fd, 
                loop_callback callback, 
                int len,
                close_callback close_cb,
                uint8_t cfg);

#endif

typedef enum
{
    WIFI_STATE_NULL,
    WIFI_STATE_CONNECTED,
}wifi_status_e;

enum_error timeout_cb();