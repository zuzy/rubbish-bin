#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>

#include "cJSON.h"
#include "common.h"
#include "halo_tcp_cli.h"

typedef struct _st_tcp_cli
{
    int fd;
    char host[128];
    uint16_t port;
    struct sockaddr_in server;
}st_tcp_cli;

static st_tcp_cli s_handle = {-1, "192.168.2.9", 8888};

int init_tcp_cli()
{
    int len;
    int optval = 1;
    struct hostent *he = NULL;
    s_handle.fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(s_handle.fd, IPPROTO_TCP, TCP_NODELAY, &optval, len);
    if(s_handle.fd < 0) {
        print_cli("fd is illegal");
        return -1;
    }
    memset(&s_handle.server, 0, sizeof(s_handle.server));
    s_handle.server.sin_family = AF_INET;
    s_handle.server.sin_port = htons(s_handle.port);
    he = gethostbyname(s_handle.host);
    s_handle.server.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    if(connect(s_handle.fd, (struct in_addr *)&s_handle.server, sizeof(s_handle.server)) < 0) {
        print_cli("connect failed");
        close(s_handle.fd);
        return -1;
    }
    print_cli("%d tcp client connected!", s_handle.fd);
    return s_handle.fd;
}

int tcp_cli_recv_cb(void *arg)
{
    st_loop *loop = (st_loop *)arg;
    memset(loop->body.payload, 0, loop->body.max_len);
    loop->body.len = recv(loop->fd, loop->body.payload, loop->body.max_len, 0);
    if(loop->body.len <= 0) {
        if(errno == EINTR) {
            return 0;
        }
        return -1;
    }
}

int tcp_cli_close(int fd)
{
    print_cli("client close!");
    return close(fd);
}