#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "common.h"

#include "halo_udp.h"
#include "cJSON.h"


static st_loop s_loop;

typedef struct _st_udp{
    int fd;
    struct sockaddr_in localSock;
    struct ip_mreq group;
}st_udp;

static st_udp s_udp_handle;

int halo_init_udp()
{
    s_udp_handle.fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(s_udp_handle.fd < 0) {
        print_udp("Opening datagram socket error");
        return -1;
    } else
        print_udp("Opening datagram socket....OK.");
  
    int opt = SO_REUSEADDR;
    setsockopt(s_udp_handle.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset((char *) &s_udp_handle.localSock, 0, sizeof(s_udp_handle.localSock));
    s_udp_handle.localSock.sin_family = AF_INET;
    s_udp_handle.localSock.sin_port = htons(19601);
    s_udp_handle.localSock.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(s_udp_handle.fd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&s_udp_handle.localSock, sizeof(s_udp_handle.localSock));

    if(bind(s_udp_handle.fd, (struct sockaddr*)&s_udp_handle.localSock, sizeof(s_udp_handle.localSock))){
        print_udp("Binding datagram socket error");
        close(s_udp_handle.fd);
        return -1;
    } else
        print_udp("Binding datagram socket...OK.");

    s_udp_handle.group.imr_multiaddr.s_addr = inet_addr("224.0.0.1");
    s_udp_handle.group.imr_interface.s_addr = inet_addr("127.0.0.1");

    // group.imr_interface.S_un.s_addr = inet_addr("127.0.0.1");
    // group.imr_multiaddr.S_un.s_addr = inet_addr("224.0.0.1");

    if(setsockopt(s_udp_handle.fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&s_udp_handle.group, sizeof(s_udp_handle.group)) < 0){
        print_udp("Adding multicast group error");
        close(s_udp_handle.fd);
        return -1;
    } else
        print_udp("Adding multicast group...OK.");
    return s_udp_handle.fd;
}

int halo_udp_close_cb(int fd)
{
    return close(fd);
}

static int __dispatch(st_body *body, st_cmd *cmd)
{
    return 0;
}

int halo_udp_recv_cb(void *arg)
{
    st_loop *loop = arg;
    if(loop->fd < 0) {
        return -1;
    }
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    loop->body.len = recvfrom(loop->fd, loop->body.payload, loop->body.max_len, 0, (struct sockaddr *)&client, &len);
    if(loop->body.len > 0) {
        print_udp("udp recv %d, %s", loop->body.len, loop->body.payload);
        return __dispatch(&loop->body, &loop->cmd);
    } else {
        if(errno == EINTR) {
            return 0;
        }
    }
}
