#ifndef __HALO_TCP_SRV__
#define __HALO_TCP_SRV__

#include "common.h"

#define HALO_TCP_SRV_DEBUG 1
#if HALO_TCP_SRV_DEBUG == 1
    #define print_srv(format, arg...)   do { printf("\033[31m[halo_srv]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_srv(format, arg...)   do {} while (0)
#endif

#define HALO_MAX_CLIENT     10

int init_tcp_server(int port);
int tcp_srv_close_cb(int fd);
enum_error tcp_srv_send_cb(int fd, void *arg);
int tcp_srv_recv_cb(void *arg);
int tcp_accept_cb(void *arg);

#endif