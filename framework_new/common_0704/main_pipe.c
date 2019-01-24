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
#include "main_loop.h"
#include "main_pipe.h"

static int p[2];
int init_pipe(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    if(pipe(p) < 0) {
        return -1;
    }
    pro->fd = p[0];
    pro->active = 1;
    print_pipe("pipe init");
    return 0;
}

int dispatch(st_proto *pro)
{
    return 0;
}

int recv_pipe(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    int len = read(pro->fd, pro->recv_buf.payload, pro->recv_buf.max_len);
    if(len > 0) {
        print_pipe("recv: %d, %s", pro->recv_buf.len, pro->recv_buf.payload);
        dispatch(pro);
        update_status(pro->hal_cmd);
        msg_apps_t msg;
        msg.type = 1;
        sprintf(msg.content, "hello\n");
        add_msg(pro->msgs, "tcp_cli", msg);
        add_msg(pro->msgs, "tcp_cli_sdss", msg);
        return 0;
    }
    return -1;
}

int close_pipe(void *arg)
{
    print_pipe("close pipe");
    st_proto *pro = (st_proto *)arg;
    close(p[0]);
    close(p[1]);
    pro->fd = -1;
    pro->active = 0;
    return 0;
}

static st_callbacks cbs = {
    init_pipe,
    recv_pipe,
    NULL,
    NULL,
    NULL,
    NULL,
    close_pipe,
};

int update_pipe(void *arg)
{
    write(p[1], "hello\n", 6);
    return 0;
}

int add_pipe()
{
    st_proto *pro = malloc(sizeof(st_proto));
    memset(pro, 0, sizeof(st_proto));
    strcpy(pro->name, "syspipe");
    pro->config |= LOOP_CFG_RETRY;
    pro->recv_buf.payload = malloc(MAX_PIPE_PAYLOAD_LEN);
    pro->recv_buf.max_len = MAX_PIPE_PAYLOAD_LEN;
    pro->recv_buf.len = 1;
    pro->callbacks = &cbs;
    return add_proto(halo_handle, pro);
}