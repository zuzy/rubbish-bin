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

struct _st_tcp_ser{
    int port;
    int block;
}tcp_ser_handle = {
    12315,
    10,
};



int tcp_ser_init(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    struct sockaddr_in server;
    int opt = SO_REUSEADDR;
    print_ser();
    pro->fd = socket(AF_INET, SOCK_STREAM, 0);
    REQUIRE((pro->fd < 0), Error);
    setsockopt(pro->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    server.sin_family = AF_INET;
    server.sin_port = htons(tcp_ser_handle.port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    REQUIRE((bind(pro->fd, (struct sockaddr *)&server, (socklen_t)sizeof(server)) < 0), Error);
    REQUIRE((listen(pro->fd, tcp_ser_handle.block) < 0), Error);
    set_nonblocking(pro->fd);
    pro->active = 1;
    print_ser("init tcp ser ok!");
    return 0;

Error:
    print_ser("tcp server init error!");
    if(pro->fd >= 0)
        close(pro->fd);
    pro->fd = -1;
    pro->active = 0;
    return -1;
}

int tcp_ser_accept(void *arg)
{
    struct sockaddr_in client;
    st_proto pro_client;
    socklen_t len = sizeof(client);
    st_proto *pro = (st_proto *)arg;
    int fd = -1;
    fd = accept(pro->fd, (struct sockaddr *)&client, &len);
    if(fd >= 0) {
        add_tcp_ser_cli(fd, NULL);
    }
    return 0;
}

int tcp_ser_close(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    if(tsc_active_num()) {
        return -1;
    }
    if(pro != NULL && pro->fd >= 0) {
        close(pro->fd);
        pro->fd = -1;
        pro->active = 0;
    }
    return 0;
}

static st_callbacks tcp_ser_cbs = {
    tcp_ser_init,
    tcp_ser_accept,
    NULL,
    NULL,
    NULL,
    NULL,
    tcp_ser_close,
};

int add_tcp_ser()
{
    st_proto *pro = (st_proto *)malloc(sizeof(st_proto));
    memset(pro, 0, sizeof(st_proto));
    strcpy(pro->name, "tcp_ser");
    pro->config |= LOOP_CFG_RETRY | LOOP_CFG_WIFI;
    pro->callbacks = &tcp_ser_cbs;
    return add_proto(halo_handle, pro);
}

int tcp_ser_port(void)
{
    // return 12316;
    return tcp_ser_handle.port;
}