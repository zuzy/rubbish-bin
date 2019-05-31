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
#include "tcp_client.h"

struct _st_tcp_client{
    char host[256];
    int port;
    int max_size;
}tcp_handle = {
    "192.168.31.246",
    8888,
    1024
};

#if 0
{
	"code": 100000,
	"message": "成功！",
	"object": {
		"authCode": "2B6DFE887320407D9F64B634415E4D01",
		"refrenceId": 754057826917978100
	}
}
#endif

static int _check_conn_is_ok(int sockfd){
    struct timeval t;
    fd_set fds;
    int error;
    socklen_t len;
    while(1){
        t.tv_sec = 5;
        t.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        if(select(sockfd + 1, &fds, NULL, NULL, &t) <= 0){
            if(errno == EINTR){
                continue;
            }
            return -1;
        }
        if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
            return -1;
        }
        return 0;
    }
}


static int __connect(int cli_fd, struct sockaddr_in server_addr, int isblock)
{
    if(isblock == 0) {
        int flags = fcntl(cli_fd,F_GETFL,0);
        fcntl(cli_fd,F_SETFL,flags | O_NONBLOCK);

        int n = connect(cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if(n < 0) {
            REQUIRE((errno != EINPROGRESS && errno != EWOULDBLOCK), Exit);

            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 100000;
            fd_set wset;
            FD_ZERO(&wset);
            FD_SET(cli_fd,&wset);
            n = select(cli_fd+1,NULL,&wset,NULL,&tv);
            REQUIRE((n <= 0), Exit);
            print_cli("Connectd.");
        }
    } else {
        //fcntl(cli_fd,F_SETFL,flags & ~O_NONBLOCK);

        //def connect timeout 75s
        REQUIRE((connect(cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0), Exit);
        print_cli("Connect OK!\n");
    }
    return cli_fd;
Exit:
    if(cli_fd >= 0) {
        close(cli_fd);
    }
    cli_fd = -1;
    return -1;
}


int tcp_cli_init(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    // int optval = 1;
    pro->fd = socket(AF_INET, SOCK_STREAM, 0);
    // setsockopt(pro->fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
    struct hostent *he;
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(tcp_handle.port);
    he = gethostbyname(tcp_handle.host);
    server.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
#if 1
    set_nonblocking(pro->fd);
    pro->fd = __connect(pro->fd, server, 0);
    
    REQUIRE((pro->fd < 0), Error);

#else
    if(connect(sockfd, (struct in_addr*)&server, sizeof(server)) < 0){
        if(errno == EINTR){
            if(_check_conn_is_ok(sockfd) == 0){
                // print_zizy("conn ok");
                goto Conn_ok;
            }
        }
        print_zizy("connect failed");
        goto Exit_client_proc;
    }
#endif
    set_nonblocking(pro->fd);
    pro->active = 1;
    print_cli("connect ok");
    return 0;
Error:
    print_cli("tcp client init error!");
    if(pro->fd >= 0) {
        close(pro->fd);
    }
    pro->fd = -1;
    pro->active = 0;
    return -1;

}

int tcp_cli_close(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_cli("close tcp cli");
    if(pro != NULL && pro->fd >= 0) {
        close(pro->fd);
        pro->active = 0;
        pro->fd = -1;
        memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
        pro->recv_buf.len = 0;
        return 0;
    }
    return -1;
}

static int _dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
    pro->recv_buf.len = 0;
    return 0;
}

int tcp_cli_recv(void *arg)
{
    int len = 0;
    st_proto *pro = (st_proto *)arg;
    len = recv(pro->fd, pro->recv_buf.payload + pro->recv_buf.len, pro->recv_buf.max_len, 0);
    if(len > 0) {
        pro->recv_buf.len += len;
        pro->recv_buf.payload[pro->recv_buf.len] = 0;
        print_cli("recv:%d %s", pro->recv_buf.len, pro->recv_buf.payload);
        // _dispatch(pro);
        return 0;
    } else {
        if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return -1;
    }
}

int tcp_cli_reply(void *arg)
{
    print_cli("reply!");
    st_proto *pro = (st_proto *)arg;
    if(pro->fd >= 0 && pro->active) {
        if(send(pro->fd, pro->recv_buf.payload, pro->recv_buf.len, 0) < 0) {
            return -1;
        }
        _dispatch(pro);
        return 0;
    }
    return -1;
}
char hb[] = "heartbeat\n";
int tcp_cli_to(void *arg)
{
    print_cli("heartbeat!");
    // return 0;
    st_proto *pro = (st_proto *)arg;
    if(pro->fd >= 0 && pro->active) {
        if(send(pro->fd, hb, strlen(hb), 0) < 0) {
            return -1;
        }
        return 0;
    }
    return -1;
}

char act[] = "active send\n";
int tcp_cli_act(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_cli("active send!");
    if(pro->fd >= 0 && pro->active) {
        if(send(pro->fd, act, strlen(act), 0) < 0) {
            return -1;
        }
        return 0;
    }
    return -1;
}
char proto_str[] = "dispatch proto\n";
int tcp_cli_dispro(void *arg, void *arg1)
{
    st_proto *pro = (st_proto *)arg;
    st_msg *msg = (st_msg *)arg1;
    if(pro->fd >= 0 && pro->active) {
        // if(send(pro->fd, proto_str, sizeof(proto_str), 0) < 0) {
        if(send(pro->fd, msg->content, strlen(msg->content), 0) < 0) {
            return -1;
        }
        print_cli("dispatch protocal\n\t%s: %ld %d %s",msg->target, strlen(msg->content), msg->type, msg->content);
        print_cli("send success!");
        return 0;
    }
    return -1;
}

static st_callbacks cli_cbs = {
    tcp_cli_init,
    tcp_cli_recv,
    tcp_cli_reply,
    tcp_cli_act,
    tcp_cli_dispro,
    tcp_cli_to,
    tcp_cli_close,
};

int add_tcp_cli()
{
    st_proto *pro = (st_proto *)malloc(sizeof(st_proto));
    memset(pro, 0, sizeof(st_proto));
    strcpy(pro->name, "tcp_cli");
    pro->config |= LOOP_CFG_RETRY | LOOP_CFG_ALINK;
    pro->recv_buf.payload = (char *)malloc(tcp_handle.max_size);
    pro->recv_buf.max_len = tcp_handle.max_size;
    pro->callbacks = &cli_cbs;
    pro->timeout = 5;
    pro->timecount = get_now_s();
    return add_proto(halo_handle, pro);
}