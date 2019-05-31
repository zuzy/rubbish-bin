#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include "cJSON.h"
#include "common.h"
#include "halo_tcp_srv.h"

typedef struct _st_tcp_srv{
    int fd;
    uint16_t port;
    struct sockaddr_in server;
}st_tcp_srv;

static st_tcp_srv s_t_handle;

int init_tcp_server(int port)
{
    s_t_handle.fd = socket(AF_INET, SOCK_STREAM, 0);
    if(s_t_handle.fd < 0) {
        print_srv("socket init error");
        return -1;
    }
    s_t_handle.port = port;
    memset(&s_t_handle.server, 0, sizeof(s_t_handle.server));
    s_t_handle.server.sin_family = AF_INET;
    s_t_handle.server.sin_port = htons(s_t_handle.port);
    s_t_handle.server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(s_t_handle.fd, (struct sockaddr *)&s_t_handle.server, sizeof(s_t_handle.server)) < 0) {
        print_srv("bind failed");
        close(s_t_handle.fd);
        return -1;
    }
    if(listen(s_t_handle.fd, HALO_MAX_CLIENT) < 0) {
        print_srv("listen failed");
        close(s_t_handle.fd);
        return -1;
    }
    print_srv("srv init ok %d", s_t_handle.fd);
    
    return s_t_handle.fd;
}

int tcp_srv_close_cb(int fd)
{
    print_srv("%d close", fd);
    return close(fd);
}

enum_error tcp_srv_send_cb(int fd, void *arg)
{
    st_cmd *cmd = (st_cmd *)arg;
    // send!!!!
    return ERR_IGN;
}

int tcp_srv_recv_cb(void *arg)
{
    st_loop *loop = (st_loop *)arg;
    memset(loop->body.payload, 0, loop->body.max_len);
    loop->body.len = recv(loop->fd, loop->body.payload, loop->body.max_len, 0);
    if(loop->body.len <= 0) {
        if(errno == EINTR) {
            return 0;
        } else {
            return -1;
        }
    } else {
        print_srv("%d:%s", loop->body.len, loop->body.payload);
        send(loop->fd, loop->body.payload, loop->body.len, 0);
        // dispatch(cmd)
    }
    return 0;
}

int tcp_accept_cb(void *arg)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int connfd;
    st_loop *loop = (st_loop *)arg;
    if((connfd = accept(loop->fd, (struct sockaddr *)&client, &len)) != -1){
        print_srv("get client");
        halo_loop_add(connfd, tcp_srv_recv_cb, 1024, tcp_srv_close_cb);
        halo_send_add(connfd, tcp_srv_send_cb, 1024);
    }
}