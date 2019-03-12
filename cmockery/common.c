#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <stdarg.h>
#include "list.h"
#include "common.h"
#include <cmockery.h>
#define UNIT_TESTING 1
#define main example_main
typedef struct _st_halo_common
{
    pthread_t tid;
    int pipefd[2];
    struct list_head *inits;
    struct list_head *loops;
    struct list_head *sends;
    common_callback loop_callback;
    common_callback timeout_callback;
    struct timeval timeout;
} st_halo_common;

static st_halo_common *s_handle = NULL;

static int __common_init_add(struct list_head *head, st_init *init)
{
    print_common("add init !");
    struct _st_init *p_init, *tmp;
    struct list_head *h = head;
    if (init->callback == NULL)
    {
        print_common("callback is null");
        return -1;
    }
    list_for_each_entry_safe(p_init, tmp, head, list)
    {
        // print_common();
        if (p_init->callback == init->callback)
        {
            print_common("this callback already inside!");
            return 0;
        }
    }
    list_add_tail(&init->list, h);
    print_common("add init list");
    return 0;
}

int halo_init_add(init_callback callback, void *arg)
{
    if (s_handle == NULL)
    {
        print_common("common handle need to init");
        return -1;
    }
    st_init *init = (st_init *)malloc(sizeof(st_init));
    if (init == NULL)
    {
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
    if (send->fd < 0 || send->send_cb == NULL)
    {
        print_common("send is illegal");
        return -1;
    }
    print_common("common send add!");
    list_for_each_entry(tmp, head, list)
    {
        if (send->fd == tmp->fd)
        {
            print_common("fd: %d is exited\n", tmp->fd);
            halo_send_del(send);
            return 0;
        }
    }
    list_add_tail(&send->list, h);
    return 0;
}

int halo_send_add(int fd, send_callback callback, int len)
{
    if (s_handle == NULL)
    {
        print_common("common handle need to init!");
        return -1;
    }
    st_send *s = (st_send *)malloc(sizeof(st_send));
    if (s == NULL)
    {
        print_common("malloc err");
        return -1;
    }
    s->fd = fd;
    s->send_cb = callback;
    s->body.payload = malloc(len);
    s->body.max_len = len;
    s->body.p_body = s->body.payload;

    if (__common_send_add(s_handle->sends, s))
    {
        goto Error;
    }
    return 0;
Error:
    if (s != NULL)
    {
        free(s);
        s = NULL;
    }
    return -1;
}

int halo_send_del(st_send *send)
{
    if (send == NULL)
    {
        print_common("target loop is null!");
        return -1;
    }
    print_common("start to del loop");

    if (send->body.payload != NULL)
    {
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

static int __common_loop_add(struct list_head *head, st_loop *loop)
{
    st_loop *tmp;
    struct list_head *h = head;
    if (loop->fd < 0 || loop->callback == NULL)
    {
        print_common("loop is illegal!");
        return -1;
    }
    list_for_each_entry(tmp, head, list)
    {
        print_common();
        if (loop->fd == tmp->fd)
        {
            print_common("fd %d is exited\n", loop->fd);
            return 0;
        }
    }
    list_add_tail(&loop->list, h);
    return 0;
}

int halo_loop_add(int fd,
                  loop_callback callback,
                  int len,
                  close_callback close_cb)
{
    if (s_handle == NULL)
    {
        print_common("common handle need to init!");
        return -1;
    }
    st_loop *loop = (st_loop *)malloc(sizeof(st_loop));
    if (loop == NULL)
    {
        print_common("MALLOC ERROR");
        goto Error;
    }
    loop->fd = fd;
    loop->callback = callback;
    loop->body.payload = malloc(len);
    loop->body.max_len = len;
    loop->body.p_body = loop->body.payload;
    loop->close_cb = close_cb;
    if (__common_loop_add(s_handle->loops, loop))
    {
        goto Error;
    }
    return 0;
Error:
    if (loop != NULL)
    {
        free(loop);
        loop = NULL;
    }
    return -1;
}

int halo_loop_del(st_loop *loop)
{
    if (loop == NULL)
    {
        print_common("target loop is null!");
        return -1;
    }
    print_common("start to del loop");
    if (loop->fd >= 0 && loop->close_cb != NULL)
    {
        print_common("close fd!");
        loop->close_cb(loop->fd);
    }
    print_common("close ok");
    if (loop->body.payload != NULL)
    {
        free(loop->body.payload);
        loop->body.payload = NULL;
    }

    print_common("free ok");
    list_del(&loop->list);
    free(loop);
    loop = NULL;
    print_common("list del ok");
    return 0;
}

int halo_common_del(int fd)
{
    st_loop *p_loop, *t_loop;
    list_for_each_entry_safe(p_loop, t_loop, s_handle->loops, list)
    {
        if (p_loop->fd == fd)
        {
            print_common("recv fd:%d -> close && delete", fd);
            halo_loop_del(p_loop);
            break;
        }
    }
    st_send *p_send, *t_send;
    list_for_each_entry_safe(p_send, t_send, s_handle->sends, list)
    {
        if (p_send->fd == fd)
        {
            print_common("send fd:%d ->del", fd);
            halo_send_del(p_send);
        }
    }
    return 0;
}

int halo_common_send(st_cmd cmd)
{
    enum_error el;
    if (s_handle == NULL || s_handle->sends == NULL)
    {
        print_common("handle is null or send list is null");
        return -1;
    }
    st_send *p_send, *t_send;
    list_for_each_entry_safe(p_send, t_send, s_handle->sends, list)
    {
        if (p_send->fd >= 0 && p_send->send_cb != NULL)
        {
            el = p_send->send_cb(p_send->fd, &cmd);
            if (el == ERR_CLOSE)
            {
                halo_common_del(p_send->fd);
            }
        }
    }
    return 0;
}

int pipe_callback(void *arg)
{
    st_loop *loop = (st_loop *)arg;

    read(loop->fd, &loop->cmd, sizeof(st_cmd));
    print_common("cmd get: %d", loop->cmd.cmd);
    halo_common_send(loop->cmd);
    return 0;
}

static struct list_head inits = LIST_HEAD_INIT(inits);
static struct list_head loops = LIST_HEAD_INIT(loops);
static struct list_head sends = LIST_HEAD_INIT(sends);

int halo_common_init(common_callback loop_cb,
                     common_callback to_cb,
                     unsigned long sec,
                     unsigned long usec)
{
    s_handle = (st_halo_common *)malloc(sizeof(st_halo_common));
    if (s_handle == NULL)
    {
        print_common("MALLOC ERROR");
        goto Error;
    }
    s_handle->inits = &inits;
    s_handle->loops = &loops;
    s_handle->sends = &sends;
    s_handle->loop_callback = loop_cb;
    s_handle->timeout_callback = to_cb;
    s_handle->timeout.tv_sec = sec;
    s_handle->timeout.tv_usec = usec;
    memset(s_handle->pipefd, 0, 2 * sizeof(int));
    if (pipe(s_handle->pipefd) < 0)
    {
        goto Error;
    }
    halo_loop_add(s_handle->pipefd[0], pipe_callback, sizeof(st_cmd), NULL);
    print_common("pipe init ok");
    return 0;
Error:
    if (s_handle != NULL)
    {
        list_del(s_handle->inits);
        list_del(s_handle->loops);
        list_del(s_handle->sends);
        if (s_handle->pipefd[0])
            close(s_handle->pipefd[0]);
        if (s_handle->pipefd[1])
            close(s_handle->pipefd[1]);
        free(s_handle);
        s_handle = NULL;
    }
    return -1;
}

int halo_common_deinit()
{
    if (s_handle == NULL)
    {
        return 0;
    }
    if (s_handle->inits != NULL)
    {
        st_init *p_init, *t_init;
        print_common("deinit_inits");
        list_for_each_entry_safe(p_init, t_init, s_handle->inits, list)
        {
            if (!p_init)
                continue;
            list_del(&p_init->list);
            free(p_init);
            p_init = NULL;
        }
    }
    print_common("deinit inits ok");
    if (s_handle->loops != NULL)
    {
        st_loop *p_loop, *t_loop;
        print_common("deinit loops");
        list_for_each_entry_safe(p_loop, t_loop, s_handle->loops, list)
        {
            halo_loop_del(p_loop);
        }
    }
    print_common("deinit loops ok");

    if (s_handle->sends != NULL)
    {
        st_send *p_send, *t_send;
        print_common("deinit sends");
        list_for_each_entry_safe(p_send, t_send, s_handle->sends, list)
        {
            halo_send_del(p_send);
        }
    }
    print_common("deinit sends ok");
    close(s_handle->pipefd[0]);
    close(s_handle->pipefd[1]);
    print_common("close pipes ok");
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
    list_for_each_entry_safe(p_loop, t_loop, loops, list)
    {
        if (p_loop->fd < 0 || p_loop->callback == NULL)
        {
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
    list_for_each_entry_safe(p_loop, t_loop, loops, list)
    {
        if (FD_ISSET(p_loop->fd, fds))
        {
            return p_loop;
        }
    }
    return NULL;
}

static void *__common_proc(void *arg)
{
    pthread_detach(pthread_self());
    if (s_handle == NULL)
    {
        print_common("COMMON HANDLE NEED TO INIT!");
        return NULL;
    }
    enum_error e = ERR_IGN;
    st_init *p_init, *t_init;
    st_loop *p_loop;
    fd_set fds;
    int max_fd, sel_ret;
    struct timeval t;
Init:
    list_for_each_entry_safe(p_init, t_init, s_handle->inits, list)
    {
    Init_Retry:
        e = p_init->callback(p_init->arg);
        if (e != ERR_IGN)
        {
            if (e == ERR_FORCE_LOCALE)
            {
                goto Init_Retry;
            }
            else
            {
                break;
            }
        }
    }
    switch (e)
    {
    case ERR_IGN:
        break;
    case ERR_FORCE_LOCALE:
    case ERR_FORCE_INIT:
    {
        goto Init;
    }
    case ERR_FORCE_REBOOT:
    {
        // system("reboot");
        break;
    }
    default:
        break;
    }

    while (1)
    {
        max_fd = ___regist_loop(&fds, s_handle->loops);
        // print_common("timeout is %ld %ld", s_handle->timeout.tv_sec, s_handle->timeout.tv_usec);
        t.tv_sec = s_handle->timeout.tv_sec;
        t.tv_usec = s_handle->timeout.tv_usec;
        sel_ret = select(max_fd + 1, &fds, NULL, NULL, &t);
        if (sel_ret > 0)
        {
            p_loop = __find_loop(&fds, s_handle->loops);
            if (p_loop == NULL)
            {
                print_common("target loop cannot find");
            }
            else
            {
                p_loop->callback(p_loop);
                print_common("GET CMD %d", p_loop->cmd.cmd);
            }
        }
        else
        {
            if (errno == EINTR)
            {
                continue;
            }
            if (sel_ret == 0)
            {
                e = s_handle->timeout_callback();
                if (e != ERR_IGN)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    if (e == ERR_FORCE_INIT)
    {
        goto Init;
    }
    else
    {
        print_common("err %d", e);
    }
}

int halo_common_start()
{
    if (s_handle == NULL)
    {
        print_common("common need to init");
        return -1;
    }
    pthread_create(&s_handle->tid, NULL, __common_proc, NULL);
}

int halo_common_stop()
{
    pthread_cancel(s_handle->tid);
    usleep(1000);
    // pthread_exit(s_handle->tid);
    return halo_common_deinit();
}

int halo_update(st_cmd cmd)
{
    if (s_handle == NULL)
    {
        return -1;
    }
    write(s_handle->pipefd[1], (void *)&cmd, sizeof(cmd));
    return 0;
}

enum_error timeout_cb()
{
    print_common("time out !!!");
    // halo_common_deinit();
    return ERR_IGN;
}

static int initcb(void *arg)
{
    print_common("init !!!!!!");
    return 0;
}

static int initcb2(void *arg)
{
    print_common("init2 !!!!!!");
    return 0;
}
static int initcb3(void *arg)
{
    print_common("init3 !!!!!!");
    return 0;
}
static st_cmd loop_cb(void *arg)
{
    st_cmd cmd;
    return cmd;
}

void *test_proc(void *arg)
{
    st_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));
    while (1)
    {
        halo_update(cmd);
        sleep(3);
        cmd.cmd++;
    }
}

#if 1
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
#if 0
    struct list_head head = LIST_HEAD_INIT(head);
    static int index = 0;
    st_init *ptr = malloc(sizeof(st_init));
    ptr->arg = calloc(1, 10);
    sprintf(ptr->arg, "%d", index);
    print_common();
    list_add(&ptr->list, &head);
    print_common();
    list_for_each_entry(ptr, &head,list) {
        if(ptr) {
            print_common("index %s\n", (char *)ptr->arg);
        } else {
            print_common("22222");
        }
        sleep(1);
        if(index < 10){
            st_init *p = malloc(sizeof(st_init));
            p->arg = calloc(1, 10);
            sprintf(p->arg, "%d", ++index);
            list_add(&p->list, &ptr->list);
        }
    }
    return 0;

#else
    halo_common_init(NULL, timeout_cb, 2, 0);
    halo_init_add(initcb, NULL);
    halo_init_add(initcb2, NULL);
    halo_init_add(initcb3, NULL);
    // halo_loop_add(1, loop_cb, 0, 10, NULL);
    halo_common_start();
    print_common("helo");
    st_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));
    do
    {
        halo_update(cmd);
        // sleep(1);
        cmd.cmd++;
        // break;
    } while (0);
    // pthread_cancel(s_handle->tid);
    // sleep(1);
    // return 0;
    halo_common_stop();
// halo_common_deinit();
#endif
    return 0;
}
#else

#endif