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

// #include "smart_player.h"
#include "smart_player_api.h"

static int p[2] = {-1, -1};
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
    splayer_status_t status;
    #if 0
    int len = sizeof(status);
    int i, num;
    num = pro->recv_buf.len / sizeof(status);
    for(i = 0; i < num; i++) {
        memcpy(&status, pro->recv_buf.payload, sizeof(status));
        mvforward(pro->recv_buf.payload, pro->recv_buf.len, sizeof(status));
        pro->recv_buf.len -= sizeof(status);
        update_status(&status);
    }
    #else
    memcpy(&status, pro->recv_buf.payload, sizeof(status));
    update_status(&status);
    clear_payload(pro);
    #endif
    return 0;
}

int recv_pipe(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    int len = read(pro->fd, pro->recv_buf.payload + pro->recv_buf.len, pro->recv_buf.max_len);
    if(len > 0) {
        pro->recv_buf.len += len;
        print_pipe("recv: %d, %s", pro->recv_buf.len, pro->recv_buf.payload);
        dispatch(pro);

        // update_status();
        // st_pro_msg msg;
        // msg.type = 1;
        // sprintf(msg.content, "hello\n");
        // msg.length = strlen(msg.content);
        // add_msg(pro->msgs, "tcp_cli", msg);
        // add_msg(pro->msgs, "tcp_cli_sdss", msg);
        
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
    p[0] = -1;
    p[1] = -1;
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

int update_pipe(void *arg, int len)
{
    if(len && p[1] >= 0) {
        print_pipe("send %d", len);
        write(p[1], (char *)arg, len);
    }
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