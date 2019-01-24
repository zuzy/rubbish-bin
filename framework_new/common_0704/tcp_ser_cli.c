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
#include<fcntl.h>
#include<termios.h>
// #include "net/config.h"
// #include "net/wifi_manager.h"
#include "list.h"
#include "common.h"
#include "main_loop.h"
#include "tcp_ser.h"
#include "tcp_ser_cli.h"
#include "tcp_ser_dispatch.h"
#include "cjson_list.h"

static int tsc_buf_len = 1024;
static int client_num = 0;

static int _dispatch(void *);

int tcp_ser_cli_init(void *arg)
{
    st_proto *pro = (st_proto *)arg;
}

static int _debug_dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    if(pro->recv_buf.len > 200) {
        msg_apps_t msg;
        msg.type = 1;
        strcpy(msg.content, "hello!");
        add_msg(pro->msgs, "tcp_cli", msg);
        memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
        pro->recv_buf.len = 0;
    }
    return 0;
}

int tcp_ser_cli_recv(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    int len = 0;
    len = recv(pro->fd, pro->recv_buf.payload + pro->recv_buf.len, pro->recv_buf.max_len, 0);
    if(len > 0) {
        pro->recv_buf.len += len;
        pro->recv_buf.payload[pro->recv_buf.len] = 0;
        print_ser("recv: %d %s", pro->recv_buf.len, pro->recv_buf.payload);
        if(_dispatch(pro) == 9) {
            return -1;
        }
        clear_payload(pro);
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
const char act_str[] = "hello baby";
int tcp_ser_cli_act(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    int len = send(pro->fd, act_str, strlen(act_str), 0);
    if(len > 0)
        return 0;
    else 
        return -1;
}

int tcp_ser_cli_dispro(void *arg, void *arg1)
{
    st_proto *pro = (st_proto *)arg;
    st_msg *msg = (st_msg *)arg1;
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
    pro->timeout = 45;
    pro->timecount = get_now_s();
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

