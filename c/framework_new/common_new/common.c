#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include<fcntl.h>
#include<termios.h>
// #include "net/config.h"
// #include "net/wifi_manager.h"
#include "list.h"
#include "common.h"

#define DEBUG_LOCALE 1
#if DEBUG_LOCALE == 1
static int alink_connect = 0;
static int wifi_connect = 0;
static  int hotplug_get_wifi_status(wifi_status_e *state)
{
    if(wifi_connect) {
        *state = WIFI_STATE_CONNECTED;
    } else {
        *state = WIFI_STATE_NULL;
    }
    print_common("wifi stat: %d", *state);
    return 0;
}
static int sair_ai_dev_connected()
{
    print_common("alink %d", alink_connect);
    return alink_connect;
}
#endif
// #include "halo_udp.h"
// #include "halo_tcp_srv.h"
// #include "halo_tcp_cli.h"

typedef struct _st_halo_common
{
    pthread_t tid;
    int pipefd[2];
    struct list_head *inits;
    struct timeval timeout;
    st_loop_new loop_obj;
    common_callback loop_manager;
}st_halo_common;

static st_halo_common *s_handle = NULL;

static int __common_init_add(struct list_head *head, st_init *init)
{
    struct _st_init *p_init, *tmp;
    struct list_head *h = head;
    if(init->callback == NULL) {
        print_common("callback is null");
        return -1;
    }
    list_for_each_entry_safe(p_init, tmp, head, list) {
        // print_common();
        if(p_init->callback == init->callback){
            print_common("this callback already inside!");
            return 0;
        }
    }
    list_add_tail(&init->list, h);
    return 0;
}

int halo_init_add(init_callback callback, void *arg)
{
    if(s_handle == NULL) {
        print_common("common handle need to init");
        return -1;
    }
    st_init *init = (st_init *)malloc(sizeof(st_init));
    if(init == NULL) {
        print_common("MALLOC ERROR");
        return -1;
    }
    init->callback = callback;
    init->arg = arg;
    return __common_init_add(s_handle->inits, init);
}

static int __common_send_add(struct list_head *head, st_send *send)
{
    st_send *tmp;
    struct list_head *h = head;
    if(send->fd < 0 || send->send_cb == NULL) {
        print_common("send is illegal");
        return -1;
    }
    list_for_each_entry(tmp, head, list) {
        if(send->fd == tmp->fd) {
            print_common("fd: %d is exited\n", tmp->fd);
            return 0;
        }
    }
    list_add_tail(&send->list, h);
    return 0;
}

int halo_send_add(int fd, send_callback callback, int len)
{
    if(s_handle == NULL) {
        print_common("common handle need to init!");
        return -1;
    }
    st_send *s = (st_send *)malloc(sizeof(st_send));
    if(s == NULL) {
        print_common("malloc err");
        return -1;
    }
    s->fd = fd;
    s->send_cb = callback;
    s->body.payload = malloc(len);
    s->body.max_len = len;
    s->body.p_body = s->body.payload;

    if(__common_send_add(s_handle->loop_obj.send_list, s)) {
        goto Error;
    }

    return 0;
Error:
    if(s != NULL) {
        free(s);
        s = NULL;
    }
    return -1;
}

int halo_send_del(st_send *send)
{
    if(send == NULL) {
        print_common("target loop is null!");
        return -1;
    }
    print_common("start to del loop");
    
    if(send->body.payload != NULL) {
        free(send->body.payload);
        send->body.payload = NULL;
    }

    print_common("free ok");
    list_del(&send->list);
    free(send);
    send = NULL;
    print_common("list del ok");
    return 0;
}


static int __common_loop_init_add(struct list_head *head, st_loop_init *init)
{
    st_loop_init *tmp;
    struct list_head *h = head;
    if(init == NULL || init->loop_init_cb == NULL || init->loop_cb == NULL || init->close_cb == NULL) {
        print_common("init is illegal");
        return-1;
    }
    print_common("add");
    list_for_each_entry(tmp, head, list) {
        print_common("++");
        if(init->loop_init_cb == tmp->loop_init_cb) {
            print_common("loop is exited");
            return 0;
        }
    }
    print_common("begin to add");
    list_add_tail(&init->list, h);
    return 0;
}

int halo_loop_init_add(loop_init_callbck init_cb,
                loop_callback loop_cb, 
                close_callback close_cb,
                int len,
                uint8_t cfg)
{
    if(s_handle == NULL) {
        print_common("common handle need to init!");
        return -1;
    }
    st_loop_init *init = (st_loop_init *)malloc(sizeof(st_loop_init));
    if(init == NULL) {
        print_common("MALLOC LOOP INIT ERROR");
        goto Error;
    }
    print_common("loop init list addition");
    init->loop_init_cb = init_cb;
    init->loop_cb = loop_cb;
    init->close_cb = close_cb;
    init->len = len;
    init->cfg = cfg;
    print_common("loop init set");
    if(__common_loop_init_add(s_handle->loop_obj.init_list, init)){
        goto Error;
    }
    print_common("add ok");
    s_handle->loop_obj.update = 1;
    return 0;
Error:
    if(init != NULL){
        free(init);
        init = NULL;
    }
    return -1;
}

int halo_loop_init_del(st_loop_init *init)
{
    if(init == NULL) {
        print_common("target loop init is null!");
        return -1;
    }

    list_del(&init->list);
    free(init);
    init = NULL;
    return 0;
}

static int __common_loop_recv_add(struct list_head *head, st_loop *loop)
{
    st_loop *tmp;
    struct list_head *h = head;
    if(loop->fd < 0 || loop->callback == NULL) {
        print_common("loop is illegal!");
        return -1;
    }
    list_for_each_entry(tmp, head, list) {
        if(loop->fd == tmp->fd) {
            print_common("fd %d is exited\n", loop->fd);
            return 0;
        }
    }
    list_add_tail(&loop->list, h);
    return 0;
}

int halo_loop_recv_add(int fd, 
                loop_callback callback, 
                int len,
                close_callback close_cb,
                uint8_t cfg)
{
    if(s_handle == NULL) {
        print_common("common handle need to init!");
        return -1;
    }
    if(fd < 0) {
        print_common("fd is illegal");
        return -1;
    }
    st_loop *loop = (st_loop*)malloc(sizeof(st_loop));
    if(loop == NULL) {
        print_common("MALLOC ERROR");
        goto Error;
    }
    loop->fd = fd;
    loop->callback = callback;
    loop->close_cb = close_cb;
    loop->body.payload = malloc(len);
    loop->body.max_len = len;
    loop->body.p_body = loop->body.payload;
    loop->cfg = cfg;
    if(__common_loop_recv_add(s_handle->loop_obj.loop_list, loop)){
        goto Error;
    }
    return 0;
Error:
    if(loop != NULL){
        free(loop);
        loop = NULL;
    }
    return -1;
}

int halo_loop_del(st_loop *loop)
{
    if(loop == NULL) {
        print_common("target loop is null!");
        return -1;
    }
    if(loop->fd >= 0 && loop->close_cb != NULL) {
        loop->close_cb(loop->fd);
    }

    if(loop->body.payload != NULL) {
        free(loop->body.payload);
        loop->body.payload = NULL;
    }

    list_del(&loop->list);
    free(loop);
    loop = NULL;
    return 0;
}

int halo_common_del(int fd)
{
    st_loop *p_loop, *t_loop;
    list_for_each_entry_safe(p_loop, t_loop, s_handle->loop_obj.loop_list, list) {
        if(p_loop->fd == fd) {
            print_common("recv fd:%d -> close && delete", fd);
            halo_loop_del(p_loop);
            break;
        }
    }
    st_send *p_send, *t_send;
    list_for_each_entry_safe(p_send, t_send, s_handle->loop_obj.send_list, list) {
        if(p_send->fd == fd) {
            print_common("send fd:%d ->del", fd);
            halo_send_del(p_send);
        }
    }
    return 0;
}

int halo_common_send(st_cmd cmd)
{
    enum_error el;
    if(s_handle == NULL || s_handle->loop_obj.send_list == NULL) {
        print_common("handle is null or send list is null");
        return -1;
    }
    st_send *p_send, *t_send;
    list_for_each_entry_safe(p_send, t_send, s_handle->loop_obj.send_list, list) {
        if(p_send->fd >= 0 && p_send->send_cb != NULL){
            el = p_send->send_cb(p_send->fd, &cmd);
            if(el == ERR_CLOSE) {
                halo_common_del(p_send->fd);
            }
        }
    }
    return 0;
}

static int __pipe_callback(void *arg)
{
    st_loop *loop = (st_loop *)arg;
    
    read(loop->fd, &loop->cmd, sizeof(st_cmd));
    print_common("cmd get: %d", loop->cmd.cmd);
    halo_common_send(loop->cmd);
    return 0;
}

static int __pipe_init()
{
    if(s_handle == NULL) {
        return -1;
    }
    if(pipe(s_handle->pipefd) < 0) {
        return -1;
    }
    return s_handle->pipefd[0];
}

static int __pipe_close(int fd)
{
    if(s_handle == NULL) {
        return -1;
    }
    close(s_handle->pipefd[0]);
    close(s_handle->pipefd[1]);
    return 0;
}

static struct list_head inits = LIST_HEAD_INIT(inits);
static struct list_head loops = LIST_HEAD_INIT(loops);
static struct list_head sends = LIST_HEAD_INIT(sends);
static struct list_head loop_inits = LIST_HEAD_INIT(loop_inits);

static enum_error __halo_manager()
{
    if(s_handle == NULL) {
        print_common("shandle need to init");
        return -1;
    }

    int alink_connection = 0;
    wifi_status_e wifi_status;
    uint8_t status = 0;
    static uint8_t s_status = 0;
    int index = 0;
    int active_index = 0;

    hotplug_get_wifi_status(&wifi_status);
    alink_connection = sair_ai_dev_connected();

    if(wifi_status == WIFI_STATE_CONNECTED) {
        if(alink_connection == 1) {
            status |= LOOP_CFG_ALINK | LOOP_CFG_WIFI;
        } else {
            status |= LOOP_CFG_WIFI;
        }
    }

    timeout_cb();
    if(s_handle->loop_obj.update == 0 && status == s_status) {
        print_common("no need to update");
        return ERR_IGN;
    }
    s_status = status;
    st_loop_init *p_li, *t_li;
    st_loop *p_loop, *t_loop;
    int init = 0;
    // get wifi status
    
    list_for_each_entry_safe(p_li, t_li, s_handle->loop_obj.init_list, list) {
        init = 0;
        ++index;
        list_for_each_entry_safe(p_loop, t_loop, s_handle->loop_obj.loop_list, list) {
            if(p_li->fd == p_loop->fd) {
                if(p_li->cfg > status) {
                    halo_common_del(p_loop->fd);
                }
                init = 1;
                ++active_index;
                break;
            }
        }
        
        if(init == 0 && status >= p_li->cfg) {
            print_common("init is %d; status: %d:%d", init, status, p_li->cfg);
            p_li->fd = p_li->loop_init_cb();
            halo_loop_recv_add(p_li->fd, p_li->loop_cb, p_li->len, p_li->close_cb, p_li->cfg);
        }
    }
    if(index == active_index) {
        print_common("all loop init list is active!");
        s_handle->loop_obj.update = 0;
    }
    return 0;
}



int halo_common_init()
{
    print_common("init !");
    s_handle = (st_halo_common*)malloc(sizeof(st_halo_common));
    if(s_handle == NULL) {
        print_common("MALLOC ERROR");
        goto Error;
    }
    s_handle->inits = &inits;
    s_handle->loop_obj.init_list = &loop_inits;
    s_handle->loop_obj.loop_list = &loops;
    s_handle->loop_obj.send_list = &sends;
    s_handle->loop_manager = __halo_manager;
    s_handle->timeout.tv_sec = 1;
    s_handle->timeout.tv_usec = 0;
    memset(s_handle->pipefd, 0, 2 * sizeof(int));
    // if(pipe(s_handle->pipefd) < 0) {
    //     goto Error;
    // }
    print_common("set ok");
    if(halo_loop_init_add(__pipe_init, __pipe_callback, __pipe_close, sizeof(st_cmd), 0)) {
        goto Error;
    }
    print_common("pipe init ok");
    return 0;
Error:
    if(s_handle != NULL) {
        list_del(s_handle->inits);
        list_del(s_handle->loop_obj.init_list);
        list_del(s_handle->loop_obj.loop_list);
        list_del(s_handle->loop_obj.send_list);
        if(s_handle->pipefd[0] >= 0) close(s_handle->pipefd[0]);
        if(s_handle->pipefd[1] >= 0) close(s_handle->pipefd[1]);
        free(s_handle);
        s_handle = NULL;
    }
    return -1;
}

int halo_common_deinit()
{
    if(s_handle == NULL) {
        return 0;
    }
    if(s_handle->inits != NULL) {
        st_init *p_init, *t_init;
        print_common("deinit_inits");
        list_for_each_entry_safe(p_init, t_init, s_handle->inits, list) {
            list_del(&p_init->list);
            free(p_init);
            p_init = NULL;
        }
    }
    print_common("deinit inits ok");

    print_common("deinit loop obj");
    if(s_handle->loop_obj.init_list != NULL) {
        st_loop_init *p_init, *t_init;
        print_common("deinit loop init");
        list_for_each_entry_safe(p_init, t_init, s_handle->loop_obj.init_list, list) {
            halo_loop_init_del(p_init);
        }
    }
    print_common("deinit loop init ok");

    if(s_handle->loop_obj.loop_list != NULL) {
        st_loop *p_loop, *t_loop;
        print_common("deinit loops");
        list_for_each_entry_safe(p_loop, t_loop, s_handle->loop_obj.loop_list, list) {
            halo_loop_del(p_loop);
        }
    }
    print_common("deinit loops ok");

    if(s_handle->loop_obj.send_list != NULL) {
        st_send *p_send, *t_send;
        print_common("deinit sends");
        list_for_each_entry_safe(p_send, t_send, s_handle->loop_obj.send_list, list) {
            halo_send_del(p_send);
        }
    }
    print_common("deinit sends ok");
    free(s_handle);
    print_common("free handle ok");
    s_handle = NULL;
    return 0;
}

static int ___regist_loop(fd_set *fds, struct list_head *loops)
{
    st_loop *p_loop, *t_loop;
    int max_fd = -1;
    FD_ZERO(fds);
    list_for_each_entry_safe(p_loop, t_loop, loops, list) {
        if(p_loop->fd < 0 || p_loop->callback == NULL) {
            halo_loop_del(p_loop);
            continue;
        }
        max_fd = (max_fd > p_loop->fd) ? max_fd : p_loop->fd;
        FD_SET(p_loop->fd, fds);
    }
    return max_fd;
}

static st_loop *__find_loop(fd_set *fds, struct list_head *loops)
{
    st_loop *p_loop, *t_loop;
    list_for_each_entry_safe(p_loop, t_loop, loops, list) {
        if(FD_ISSET(p_loop->fd, fds)) {
            return p_loop;
        }
    }
    return NULL;
}

static void *__common_proc(void *arg)
{
    pthread_detach(pthread_self());
    if(s_handle == NULL) {
        print_common("COMMON HANDLE NEED TO INIT!");
        pthread_exit(-1);
    }
    print_common("commont thread begin");
    enum_error e = ERR_IGN;
    st_init *p_init, *t_init;
    st_loop *p_loop;
    fd_set fds;
    int max_fd, sel_ret, loop_ret;
    struct timeval t;

    while(1) {
        max_fd = ___regist_loop(&fds, s_handle->loop_obj.loop_list);
        // print_common("timeout is %ld %ld", s_handle->timeout.tv_sec, s_handle->timeout.tv_usec);
        // print_common("max fd is %d", max_fd);
        t.tv_sec = s_handle->timeout.tv_sec;
        t.tv_usec = s_handle->timeout.tv_usec;
        sel_ret = select(max_fd + 1, &fds, NULL, NULL, &t);
        if(sel_ret > 0) {
            p_loop = __find_loop(&fds, s_handle->loop_obj.loop_list);
            if(p_loop == NULL) {
                print_common("target loop cannot find");
            } else {
                loop_ret = p_loop->callback(p_loop);
                if(loop_ret != 0) {
                    print_common("del %d", p_loop->fd);
                    halo_common_del(p_loop->fd);
                }else{
                    print_common("%d GET CMD %d", p_loop->fd, p_loop->cmd.cmd);
                }
            }
            if(s_handle->loop_manager != NULL) {
                e = s_handle->loop_manager();
                if(e != ERR_IGN) {
                    print_common("error is %d", e);
                    // callback all close cb && del loop (wifi) nodes!
                    break;
                }
            } else {
                print_common("loop manager is null !!");
                break;
            }
        } else {
            if(errno == EINTR) {
                continue;
            }
            if(sel_ret == 0) {
                print_common("common");
                if(s_handle->loop_manager != NULL) {
                    e = s_handle->loop_manager();
                } else {
                    print_common("loop manager is null!!");
                    break;
                }
            } else {
                print_common("err!!! %d", sel_ret);
                break;
            }
        }
    }
    print_common("err %d", e);
}


static void *__inits_proc(void *arg)
{
    pthread_detach(pthread_self());
    if(s_handle == NULL) {
        print_common("need to init");
        return NULL;
    }
    print_common("init thread begin");
    st_init *p_init, *t_init;
    enum_error e;

Init:
    list_for_each_entry_safe(p_init, t_init, s_handle->inits, list) {
    Retry:
        e = p_init->callback(p_init->arg);
        // init once at the power on;
    }
    st_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));
    halo_update(cmd);
    return NULL;
}

int halo_common_start()
{
    if(s_handle == NULL) {
        print_common("common need to init");
        return -1;
    }
    pthread_t tid;
    pthread_create(&s_handle->tid, NULL, __common_proc, NULL);
    pthread_create(&tid, NULL, __inits_proc, NULL);
}

int halo_common_stop()
{
    pthread_cancel(s_handle->tid);
    // pthread_exit(s_handle->tid);
    return halo_common_deinit();
}

int halo_update(st_cmd cmd)
{
    if(s_handle == NULL) {
        return -1;
    }
    write(s_handle->pipefd[1], (void*)&cmd, sizeof(cmd));
    return 0;
}


#if 1
enum_error timeout_cb()
{
    // print_common("time out !!!");
    // halo_common_deinit();
    st_loop *p_loop, *t_loop;
    printf("-------------\n");
    list_for_each_entry_safe(p_loop, t_loop, s_handle->loop_obj.loop_list, list) {
        print_common("loop %d", p_loop->fd);
    }
    printf("-------------\n");
    return ERR_IGN;
}

static int initcb(void *arg)
{
    print_common("init !!!!!!");
    return 0;
}


static st_cmd loop_cb(void *arg)
{
    st_cmd cmd;
    return cmd;
}

void * test_proc(void *arg)
{
    st_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));
    while(1) {
        halo_update(cmd);
        sleep(3);
        cmd.cmd++;
    }
}
int p[2];
int loop_init_cb()
{
    print_common("init aaaaa!");
    pipe(p);
    int fd = p[0];
    return fd;
}

int _loop_cb(void *arg)
{
    return 0;
}

int _loop_close(int fd)
{
    close(fd);
    close(p[1]);
    print_common("%d close", fd);
    return 0;
}

int p2[2];
int _loop_init_a()
{
    print_common("init bbbbb!");
    pipe(p2);
    int fd = p2[0];
    return fd;
}

int _loop_cb_a(void *arg)
{
    return 0;
}

int _loop_close_a(int fd)
{
    close(fd);
    close(p2[1]);
    print_common("%d close", fd);
    return 0;
}

static int initcb2(void *arg)
{
    print_common("init2 !!!!!!");
    halo_loop_init_add(loop_init_cb, _loop_cb, _loop_close, 10, 1);
    return 0;
}
static int initcb3(void *arg)
{
    print_common("init3 !!!!!!");
    halo_loop_init_add(_loop_init_a, _loop_cb_a, _loop_close_a, 10, 3);
    return 0;
}

int main()
{
    // int i[2];
    // print_common("%d, %d", i[0], i[1]);
    // memset(i, 0, 2 * sizeof(int));
    // if(pipe(i) == -1) {
    //     print_common();
    // }
    // print_common("%d, %d", i[0], i[1]);
    // halo_common_init(NULL, NULL, 10, 0);
    print_common("hello");
    halo_common_init();
    halo_init_add(initcb, NULL);
    halo_init_add(initcb2, NULL);
    halo_init_add(initcb3, NULL);

    // halo_loop_add(1, loop_cb, 0, 10, NULL);
    // halo_loop_add(halo_init_udp(), halo_udp_recv_cb, 1024, halo_udp_close_cb);
    // halo_loop_add(init_tcp_server(19900), tcp_accept_cb, 1024, tcp_srv_close_cb);
    // halo_loop_add(init_tcp_cli(), tcp_cli_recv_cb, 1024, tcp_cli_close);
    halo_common_start();
    st_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));

    while(1) {
        // halo_update(cmd);
        sleep(5);
        wifi_connect = !wifi_connect;
        sleep(5);
        alink_connect = !alink_connect;
        sleep(5);
        alink_connect = !alink_connect;
        sleep(5);
        wifi_connect = !wifi_connect;
        // cmd.cmd++;
        // break;
    }
    sleep(1);
    // return 0;
    halo_common_stop();
    halo_common_deinit();
    return 0;
}
#endif