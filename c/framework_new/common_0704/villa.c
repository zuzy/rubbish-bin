#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/time.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>

#include "list.h"
#include "common.h"
#include "main_loop.h"
#include "udp_ser.h"
#include "tcp_ser.h"
#include "cJSON.h"
#include "villa.h"
#include "villa_dispatch.h"


struct _st_villa
{
    int tcp_status;
    int udp_port;
    int udp_from_port;
    char tcp_host[64];
    char tcp_port[8];
    int max_buf_len;
    struct sockaddr_in udp_from;
    struct sockaddr_in udp_bro;
} villa_handle = {
    0,
    6666,
    7777,
    "",
    "8888",
    1024,
};

static int _connect(int cli_fd, 
            struct sockaddr_in server_addr)
{
    int n = 0;
    int err;
    socklen_t len;
    fcntl(cli_fd,F_SETFL,fcntl(cli_fd,F_GETFL,0) | O_NONBLOCK);
    connect(cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    fd_set wset;
    fd_set eset;
    FD_ZERO(&wset);
    FD_SET(cli_fd,&wset);
    n = select(cli_fd+1,NULL,&wset,NULL,&tv);
    print_villa("select %d", n);
    if(n > 0) {
        if(FD_ISSET(cli_fd, &wset)) {
            // if(getsockopt(cli_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
            //     if(err != 0) {
            //         print_villa("fd %d is set, error %d", cli_fd, err);
            //         return -1;
            //     }
            // } 
            return 0;
        }
    }
Exit:
    if(cli_fd >= 0) {
        close(cli_fd);
    }
    cli_fd = -1;
    return -1;
}

static int _villa_tcp_init(st_proto *pro)
{
    static struct sockaddr_in server;
    int fd = -1;
    REQUIRE((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0, Error);
    struct hostent *he;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(villa_handle.tcp_port));
    he = gethostbyname(villa_handle.tcp_host);
    server.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    set_nonblocking(fd);
    print_villa("init fd ok %d", fd);
    if(_connect(fd, server) == 0) {
        print_villa("villa tcp ok");
        pro->fd = fd;
        pro->timeout = 10;
        return 0;
    }
Error:
    if(fd >= 0) {
        close(fd);
        fd = -1;
    }
    return -1;
}

char *to_str = "{\"type\":\"REQUEST_TCP\",\"sn\":\"hi-2017-04-26\"}";

static int _villa_udp_to(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    char out[] = "asdfghjkl;\n";
    socklen_t len = sizeof(struct sockaddr_in);

    sendto(pro->fd, to_str, strlen(to_str), 0, (struct sockaddr *)&villa_handle.udp_bro, len);
    print_villa("send to ...");
    return 0;
Error:
    return -1;
}

char *heart_beat_str = "{\"type\":\"REQUEST_HEART_BEAT\"}";

static int _villa_tcp_to(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    struct _st_villa_data data;
    memset(&data, 0, sizeof(data));
    data.guid = 0xaa;
    data.length = htons(strlen(heart_beat_str));
    data.cmd = htons(0x01);
    strcpy((char *)data.data, heart_beat_str);
    int i = 0; 
    int len = strlen(heart_beat_str) + 4;
    uint8_t *ptr = (uint8_t *)&data;
    ++ptr;
    for(i; i < len; i++) {
        data.chk ^= *ptr++;
    }
    data.data[strlen(heart_beat_str)] = data.chk;
    send(pro->fd, (char *)&data, len + 1, 0);
    return 0;
}

static int _villa_to(void *arg)
{
    if(villa_handle.tcp_status) {
        return _villa_tcp_to(arg);
    }
    return _villa_udp_to(arg); 
}

static int _parse_udp_body(st_proto *pro)
{
    print_villa("payload is %s", pro->recv_buf.payload);
    cJSON *root = REQ_JSON_PARSE(pro->recv_buf.payload, root, Error);
    
    cJSON *type = REQ_JSON_OBJ(root, type, Error);
    cJSON *data = REQ_JSON_OBJ(root, data, Error);
    cJSON *ip = REQ_JSON_OBJ(data, ip, Error);
    cJSON *port = REQ_JSON_OBJ(data, port, Error);
    cJSON *company = REQ_JSON_OBJ(data, company, Error);
    print_villa();
    strncpy(villa_handle.tcp_host, ip->valuestring, sizeof(villa_handle.tcp_host));
    
    switch(port->type) {
        case cJSON_String: {
            strncpy(villa_handle.tcp_port, port->valuestring, sizeof(villa_handle.tcp_port));
            break;
        }
        case cJSON_Number: {
            sprintf(villa_handle.tcp_port, "%d", port->valueint);
            break;
        }
        default:break;
    }

    print_villa("get ip %s port %s", villa_handle.tcp_host, villa_handle.tcp_port);
    cJSON_Delete(root);
    return 0;

Error:
    if(root != NULL) {
        cJSON_Delete(root);
    }
    NULL;
    return 1;
}

static void _dump(char *body, int len)
{
    int i;
    char *p = body;
    print_villa("dump begin:");
    for(i = 0; i < len; i++) {
        printf("%.02X ", *p++);
    }
    printf("\n[end]\n");
}

static int _villa_tcp_recv(st_proto *pro)
{
    pro->recv_buf.len =  recv(pro->fd, pro->recv_buf.payload, pro->recv_buf.max_len, 0);
    _dump(pro->recv_buf.payload, pro->recv_buf.len);
    villa_parse(pro);
    return 0;
}

static int _villa_udp_recv(st_proto *pro)
{
    struct sockaddr_in from;

    socklen_t len = sizeof(struct sockaddr_in);
    int length = recvfrom(pro->fd, pro->recv_buf.payload, pro->recv_buf.max_len, 0, (struct sockaddr *)&from, &len);
    print_villa("recv %d, %s", length, pro->recv_buf.payload);
    if(_parse_udp_body(pro) == 0) {
        if(_villa_tcp_init(pro) == 0) {
            print_villa("tcp init ok %d", pro->fd);
            villa_handle.tcp_status = 1;
            return 0;
        }
    }
    villa_handle.tcp_status = 0;
    return 1;
}

static int _villa_udp_init(st_proto *pro)
{
    int opt = 1;
    struct sockaddr_in server_addr;

    REQUIRE(((pro->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0), Error);

    memset(&villa_handle.udp_from, 0, sizeof(villa_handle.udp_from));
    villa_handle.udp_from.sin_family = AF_INET;
    villa_handle.udp_from.sin_port = htons(villa_handle.udp_from_port);
    villa_handle.udp_from.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&villa_handle.udp_bro, 0, sizeof(villa_handle.udp_bro));
    villa_handle.udp_bro.sin_family = AF_INET;
    villa_handle.udp_bro.sin_port = htons(villa_handle.udp_port);
    villa_handle.udp_bro.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    REQUIRE(setsockopt(pro->fd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)) < 0, Error);
    REQUIRE((bind(pro->fd, (struct sockaddr *)&(villa_handle.udp_from), sizeof(villa_handle.udp_from)) < 0), Error);
    print_villa("udp init ok %d", pro->fd);
    pro->active = 1;
    return 0;
Error:
    NULL;
    return -1;
}

char *jsonbuf = "{\"type\":\"RESPONSE_TCP\",\"version\":\"2.0.0.1\",\"data\":{\"ip\":\"192.168.31.246\",\"port\":8888,\"company\":\"VillaKit_16\"}";

static int _villa_init(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    return _villa_udp_init(pro);
}

static int _villa_recv(void *arg)
{
    print_villa("villa recv");
    st_proto *pro = (st_proto *)arg;
    if(villa_handle.tcp_status) {
        return _villa_tcp_recv(pro);
    }
    _villa_udp_recv(pro);
    return 0;
}

static int _villa_close(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    close(pro->fd);
    pro->fd = -1;
    memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
    pro->recv_buf.len = 0;
    pro->active = 0;
    villa_handle.tcp_status = 0;
    return 0;
}

st_callbacks cbs = {
    _villa_init,
    _villa_recv,
    NULL,
    NULL,
    NULL,
    _villa_to,
    _villa_close,
};

int add_villa_pro()
{
    // st_proto *pro =(st_proto *)malloc(sizeof(st_proto));
    st_proto *pro =(st_proto *)calloc(1, sizeof(st_proto));
    strcpy(pro->name, "vaillakit");
    pro->config |= LOOP_CFG_RETRY | LOOP_CFG_WIFI;
    pro->recv_buf.payload = calloc(1, villa_handle.max_buf_len);
    pro->recv_buf.max_len = villa_handle.max_buf_len;
    pro->callbacks = &cbs;
    pro->timeout = 3;
    pro->timecount =get_now_s();
    return add_proto(halo_handle, pro);
}

