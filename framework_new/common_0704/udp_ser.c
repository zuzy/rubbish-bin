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
#include <arpa/inet.h>
#include <netinet/ip.h>
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
#include "udp_ser.h"
#include "tcp_ser.h"
#include "cJSON.h"

struct _st_usrv
{
    int max_len;
    char local[64];
    char group[64];
    int l_port;
    int d_port;
    struct sockaddr_in send_group;
    char target_id[MAX_DID_LEN];
}udp_handle = {
    1024,
    "127.0.0.1",
    "224.0.0.1",
    19601,
    19602,
};

int udp_init(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    int opt = 0;
    struct sockaddr_in u_srv;
    struct ip_mreq group;
    pro->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(pro->fd < 0) {
        return -1;
    }
    opt = SO_REUSEADDR;
    setsockopt(pro->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    memset(&u_srv, 0, sizeof(u_srv));
    u_srv.sin_family = AF_INET;
    u_srv.sin_port = htons(udp_handle.l_port);
    u_srv.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(pro->fd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&u_srv, sizeof(u_srv));
    REQUIRE((bind(pro->fd, (struct sockaddr *)&u_srv, sizeof(u_srv)) != 0), Error);
    group.imr_multiaddr.s_addr = inet_addr(udp_handle.group);
    group.imr_interface.s_addr = inet_addr(udp_handle.local);
    REQUIRE((setsockopt(pro->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&group, sizeof(group)) < 0), Error);

    set_nonblocking(pro->fd);

    bzero(&udp_handle.send_group, sizeof(udp_handle.send_group));
    udp_handle.send_group.sin_family = AF_INET;
    udp_handle.send_group.sin_addr.s_addr = inet_addr(udp_handle.group);
    udp_handle.send_group.sin_port = htons(udp_handle.d_port);

    pro->active = 1;
    return 0;
Error:
    print_udp("udp init error!");
    close(pro->fd);
    pro->fd = -1;
    return -1;
}

static int _dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    cJSON *root = REQ_JSON_PARSE(pro->recv_buf.payload, root, Error);
    cJSON *cmd = REQ_JSON_OBJ(root, cmd, Error);
    REQUIRE((strcmp(cmd->valuestring, "hopediscover") != 0), Error);
    cJSON *params = REQ_JSON_OBJ(root, params, Error);
    cJSON *deviceid = REQ_JSON_OBJ(params, deviceid, Error);
    strncpy(udp_handle.target_id, deviceid->valuestring, MAX_DID_LEN);
    print_udp("get device id %s", udp_handle.target_id);
    cJSON_Delete(root);
    clear_payload(pro);
    root = NULL;
    return 0;
    
Error:
    print_udp("dispatch error");
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    return -1;
}

int udp_recv(void *arg)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    st_proto *pro = (st_proto *)arg;
    int l = 0;
    if(pro->fd >= 0 && pro->active) {
        l = recvfrom(pro->fd,
                    pro->recv_buf.payload + pro->recv_buf.len, 
                    pro->recv_buf.max_len, 
                    0,
                    (struct sockaddr *)&client,
                    &len);
        if(l <= 0){
            if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;
            }
            print_udp("recv from error %d", errno);
            return -1;
        } else {
            pro->recv_buf.len += l;
            print_udp("recv %d, %s", pro->recv_buf.len, pro->recv_buf.payload);
            if(_dispatch(pro) != 0)
                return 1;
            return 0;
        }
    }
    return -1;
}

static int _multisend(int fd, char *str, int len)
{
Retry:
    if(sendto(fd, str, len, 0, (struct sockaddr*)&udp_handle.send_group, sizeof(udp_handle.send_group)) < 0) {
        REQUIRE((errno == EINTR), Retry);
        return -1;
    }
    return 0;
}

char json_str[] = "{name:\"hello\", val:\"wrold\"}";

int udp_reply(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    cJSON *root, *params;
    char *out = NULL;
    REQUIRE((pro == NULL || pro->fd < 0 || pro->active == 0), Error);
    

    root = cJSON_CreateObject();
    REQUIRE((root==NULL), Error);
    cJSON_AddStringToObject(root, "cmd", "hopediscover");
    cJSON_AddItemToObject(root, "params", params = cJSON_CreateObject());
    cJSON_AddStringToObject(params, "hopeid", "12:34:56:78:90:ab");
    cJSON_AddNumberToObject(params, "hopeport", tcp_ser_port());
    cJSON_AddStringToObject(params, "hopeip", getip());
    cJSON_AddNumberToObject(params, "versioncode", 1060302);
    cJSON_AddStringToObject(params, "versionname", "1.6.3.2");
    cJSON_AddNumberToObject(params, "haloversion", 102);
    cJSON_AddNumberToObject(params, "playertype", 2);
    cJSON_AddStringToObject(params, "model", "HOPE-Q3");
    cJSON_AddStringToObject(params, "targetid", udp_handle.target_id);

    out = cJSON_PrintUnformatted(root);
    REQUIRE((out == NULL), Error);
    print_udp("repay json %s", out);

    REQUIRE((_multisend(pro->fd, out, strlen(out)) < 0), Error);

    cJSON_Delete(root);
    root = NULL;
    free(out);
    out = NULL;
    return 0;

Error:
    print_udp("error!");
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    if(out != NULL) {
        free(out);
        out = NULL;
    }
    return -1;
}

int udp_close(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    if(pro == NULL) {
        print_udp("pro is null");
        return -1;
    }
    if(pro->fd >= 0) {
        close(pro->fd);
    }
    pro->fd  = -1;
    pro->active = 0;
    memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
    pro->recv_buf.len = 0;
    return 0;
}

st_callbacks udp_cbs = {
    udp_init,
    udp_recv,
    udp_reply,
    NULL,
    NULL,
    NULL,
    udp_close,
};

int udp_service_init()
{
    st_proto *pro = (st_proto *)malloc(sizeof(st_proto));
    if(pro == NULL) {
        print_udp("malloc error");
        return -1;
    }
    memset(pro, 0, sizeof(st_proto));
    strcpy(pro->name, "udp_ser");
    pro->config |= LOOP_CFG_RETRY | LOOP_CFG_WIFI;
    pro->recv_buf.payload = (char *)malloc(udp_handle.max_len);
    pro->recv_buf.max_len = udp_handle.max_len;
    pro->callbacks = &udp_cbs;
    return add_proto(halo_handle, pro);
}