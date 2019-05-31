#ifndef __HALO_COMMON_H__
#define __HALO_COMMON_H__

#include "list.h"


#define HALO_COMMON_DEBUG 1
#if HALO_COMMON_DEBUG == 1
    #define print_common(format, arg...)   do { printf("\033[31m[halo_common]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_common(format, arg...)   do {} while (0)
#endif

#define MAX_BODY_LEN    1024

typedef int (*init_callback)(void *);

typedef struct _st_cmd
{
    int     cmd;
    char    content[16];
}st_cmd;

typedef enum _enum_error
{
    ERR_IGN = 0,
    ERR_CLOSE,
    ERR_FORCE_LOCALE,
    ERR_FORCE_INIT,
    ERR_FORCE_REBOOT,
}enum_error;

typedef int (* loop_callback)(void *);
typedef int (*close_callback)(int);


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
    size_t  len;
    char    *p_body;
    int     is_start;
}st_body;

typedef struct _st_loop
{
    struct list_head    list;
    int                 fd;
    loop_callback       callback;
    enum_error          elevel;
    st_body             body;
    st_cmd              cmd;
    close_callback      close_cb;
}st_loop;

typedef enum_error (*send_callback)(int fd, void *arg);

typedef struct _st_send
{
    struct list_head list;
    int fd;
    send_callback send_cb;
    st_body body;
}st_send;

typedef enum_error (*common_callback)(void);

/** 
 * @brief  init a init_struct and add to the static handle
 * @note   
 * @param  callback: the init function
 * @param  *arg: arg of function
 * @param  el: the result of callback
 * @param  retry: times of retry;
 * @retval 0 -> success
 */
int halo_init_add(init_callback callback, void *arg);

/** 
 * @brief  init a send_struct and add to the static handle
 * @note   
 * @param  fd: target fd
 * @param  callback: callback
 * @param  len: 
 * @retval 
 */
int halo_send_add(int fd, send_callback callback, int len);
int halo_send_del(st_send *send);
int halo_loop_add(int fd, 
                loop_callback callback, 
                int len,
                close_callback close_cb);
int halo_loop_del(st_loop *loop);
int halo_common_init(common_callback loop_cb, 
                    common_callback to_cb, 
                    unsigned long sec, 
                    unsigned long usec);
int halo_common_deinit();
int halo_update(st_cmd cmd);
#endif