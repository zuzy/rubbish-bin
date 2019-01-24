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
#include "net/config.h"
#include "net/wifi_manager.h"
#include "list.h"
#include "common.h"
#include "main_loop.h"
#include "tcp_client.h"
#include "tcp_cli_dispatch.h"
#include "halo_http_regist.h"
#include "halo_http_mlist.h"
#include "smart_player_api.h"

struct _st_tcp_client{
    char host[256];
    int port;
    int max_size;
    char auth[MAX_ID_LEN];
    char snd_id[MAX_ID_LEN];
    st_cli_head head;
}tcp_handle = {
    "192.168.2.9",
    8888,
    1024,
    "",
    "",
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
static int _send(int fd, st_cli_head head, char *body);
static int cloud_send(st_proto *pro, uint16_t cmd, char *input);
static int identify(st_proto *pro);
// static int hope_unbind(st_proto *pro);
static int heart_beat(st_proto*);

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


int tcp_connect(int cli_fd, struct sockaddr_in server_addr, int isblock)
{
    if(isblock == 0) {
        int flags = fcntl(cli_fd,F_GETFL,0);
        fcntl(cli_fd,F_SETFL,flags | O_NONBLOCK);

        int n = connect(cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if(n < 0) {

        Wait:
            REQUIRE((errno != EINPROGRESS && errno != EWOULDBLOCK), Exit);

            struct timeval tv;
            int err;
            socklen_t len;
            tv.tv_sec = 0;
            tv.tv_usec = 100000;
            fd_set wset;
            FD_ZERO(&wset);
            FD_SET(cli_fd,&wset);
            n = select(cli_fd+1,NULL,&wset,NULL,&tv);
            if(n < 0) {
                print_cli("n : %d \terror is %d", n, errno);
            }
            REQUIRE((n <= 0), Exit);
            REQUIRE(!FD_ISSET(cli_fd, &wset), Wait); 
            REQUIRE((getsockopt(cli_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0), Exit);
            REQUIRE(err != 0, Exit);
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
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    struct addrinfo *rp;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    REQUIRE(getaddrinfo(tcp_handle.host, NULL, &hints, &res) != 0, Error);
    print_cli("get info ok!!");
    for(rp = res; rp != NULL; rp = rp->ai_next) {
        struct sockaddr_in *s = (struct sockaddr_in *)rp->ai_addr;
        s->sin_port = htons(tcp_handle.port);
        pro->fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(pro->fd < 0) continue;
        // set_nonblocking(pro->fd);
        if(tcp_connect(pro->fd, *(struct sockaddr_in *)rp->ai_addr, 0) >= 0) {
            print_cli("conn ok !");
            break;
        }
        close(pro->fd);
        pro->fd = -1;
    }
    REQUIRE(rp == NULL, Error);
    freeaddrinfo(res);
    pro->active = 1;
    set_nonblocking(pro->fd);
    memcpy(tcp_handle.head.snd_id, halo_handle->ref_hex, 10);
    identify(pro);
    get_items();
    lua_call_device();
    return 0;
    Error:{
        if(res != NULL) {
            freeaddrinfo(res);
            res = NULL;
        }
        if(pro->fd >= 0) {
            close(pro->fd);
        }
        pro->fd = -1;
        pro->active = 0;
        return -1;
    }
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

static void cli_dump(char *str, int len)
{
    #if 0
    int i;
    print_cli("get str!:");
    for(i = 0; i < len; i++) {
        printf(" %.02X", str[i]);
    }
    printf("\n");
    #endif
}

static int _dispatch(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    cli_dump(pro->recv_buf.payload, pro->recv_buf.len);
    cli_dispatch(pro);
    cli_dump(pro->recv_buf.payload, pro->recv_buf.len);
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
        cli_dump(pro->recv_buf.payload, pro->recv_buf.len);
        _dispatch(pro);
        // cli_dump(pro->recv_buf.payload, pro->recv_buf.len);
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
        // _dispatch(pro);
        return 0;
    }
    return -1;
}
char hb[] = "heartbeat\n";
int tcp_cli_to(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_cli("heartbeat!");
    // reset_timer(pro);
    #if 1
    return heart_beat(pro);
    #else
    // return 0;
    st_proto *pro = (st_proto *)arg;
    if(pro->fd >= 0 && pro->active) {
        if(send(pro->fd, hb, strlen(hb), 0) < 0) {
            return -1;
        }
        return 0;
    }
    return -1;
    #endif
}

char act[] = "active send\n";
int tcp_cli_act(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_cli("active send!");
    
    if(pro->fd >= 0 && pro->active) {
        return hope_update_status(0);
        // return tcp_cli_update_status(pro);
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
        if(send(pro->fd, msg->content, msg->length, 0) < 0) {
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
    int server_ver = 0;
    get_config(CFG_HALO_SERVER_VERSION, &server_ver, sizeof(int));

    if(server_ver) {
        sprintf(tcp_handle.host, "open.nbhope.cn");
        tcp_handle.port = 6666;
    }
    st_proto *pro = (st_proto *)malloc(sizeof(st_proto));
    memset(pro, 0, sizeof(st_proto));
    strcpy(pro->name, "tcp_cli");
    pro->config |= LOOP_CFG_RETRY | LOOP_CFG_ALINK | LOOP_CFG_HOPE | LOOP_CFG_WIFI;
    pro->recv_buf.payload = (char *)malloc(tcp_handle.max_size);
    pro->recv_buf.max_len = tcp_handle.max_size;
    pro->callbacks = &cli_cbs;
    pro->timeout = (20 << 10);
    // pro->timecount = get_now_s();
    print_cli("add!! %s %d %llu %llu", pro->name, pro->config, pro->timecount, pro->timeout);
    return add_proto(halo_handle, pro);
}


#if 1
static char *_contentcat(st_cli_head head, char *body, int *length)
{
    char *str = (char*)&head;
    int len = (sizeof(head) << 1) + strlen(body) + 10;
    char *head_ptr = &head;
    char *send_buf = (char*)malloc(len);
    if(send_buf == NULL) {
        return NULL;
    }
    char *send_ptr = send_buf;
    int i, size, ret;
    uint8_t hex_checkout;
    bzero(send_buf, len);
    *send_ptr++ = 0x7e;
    size = sizeof(head);
    for(i = 0; i < size; i++) {
        if(0 == i) {
            hex_checkout = *head_ptr;
        } else {
            hex_checkout ^= *head_ptr;
        }
        if(*head_ptr != 0x7e && *head_ptr != 0x7d) {
            *send_ptr++ = *head_ptr;
        } else {
            *send_ptr++ = 0x7d;
            if(*head_ptr == 0x7e) {
                *send_ptr++ = 0x02;
            } else {
                *send_ptr++ = 0x01;
            }
        }
        ++head_ptr;
    }

    size = strlen(body);
    for(i = 0; i < size; i++) {
        hex_checkout ^= body[i];
        if(body[i] != 0x7e && body[i] != 0x7d) {
            *send_ptr++ = body[i];
        } else {
            *send_ptr++ = 0x7d;
            if(body[i] == 0x7e) {
                *send_ptr++ = 0x02;      
            } else {
                *send_ptr++ = 0x01;
            }
        }
    }

    if(hex_checkout != 0x7e && hex_checkout != 0x7d) {
        *send_ptr++ = hex_checkout;
    } else {
        *send_ptr++ = 0x7d;
        if(hex_checkout == 0x7e) {
            *send_ptr++ = 0x02;
        } else {
            *send_ptr++ = 0x01;
        }
    }
    *send_ptr = 0x7e;
    size = send_ptr - send_buf + 1;
    *length = size;
    return send_buf;
}

static int _send(int fd, st_cli_head head, char *body)
{
    int ret = 0;
    int size = 0;
    char *send_buf = _contentcat(head, body, &size);

Retry__send:
    ret = send(fd, send_buf, size, 0);
    if(ret < 0) {
        if(errno == EINTR) {
            goto Retry__send;
        }
    }

    if(send_buf != NULL)free(send_buf);
    return ret;
}

char *_build_payload(uint16_t cmd, char *data, int *len)
{
    char *body = NULL;
    tcp_handle.head.cmd = htons(cmd);
    tcp_handle.head.type = 1;
    tcp_handle.head.len = htons(strlen(data) << 1);
    body = _contentcat(tcp_handle.head, data, len);
    return body;
}

static int cloud_send(st_proto *pro, uint16_t cmd, char *input)
{
    if(pro->fd <= 0 || pro->active == 0) {
        return -1;
    }
    tcp_handle.head.cmd = htons(cmd);
    tcp_handle.head.type = 1;
    tcp_handle.head.len = htons(strlen(input) << 1);
    int ret = _send(pro->fd, tcp_handle.head, input);
    if(ret <= 0) {
        return -1;
    }
    return 0;
}

static int identify(st_proto *pro)
{
    char *out = NULL;
    cJSON *root = cJSON_CreateObject();
    if(root == NULL){
        return -1;
    }
    cJSON_AddStringToObject(root, "code", halo_handle->auth);
    out = cJSON_PrintUnformatted(root);
    print_cli("identify! %s", out);
    int ret = cloud_send(pro, (uint16_t)C_DEV_IDENTIFY, out);
    if(root != NULL)cJSON_Delete(root);
    if(out != NULL)free(out);
    return ret;
}

#if 0
static int hope_unbind(st_proto *pro)
{
    print_cli("unbind!!!");
    return cloud_send(pro, (uint16_t)C_DEV_UNBIND, "");
}
#else
int hope_unbind()
{
    int size;
    char *buf = NULL;
    print_cli("hope dev unbind");
    st_pro_msg msg;
    msg.type = C_DEV_UNBIND;
    buf = _build_payload(C_DEV_UNBIND, "", &size);
    memcpy(msg.content, buf, size);
    msg.length = size;
    add_msg(halo_handle->msgs, "tcp_cli", msg);
    free(buf);
    buf = NULL;
    return 0;
}
#endif

static int heart_beat(st_proto *pro)
{
    print_cli("heatrbeat");
    return cloud_send(pro, (uint16_t)C_DEV_HEART, "");
}


static cJSON *_json_of_status(void *arg, int force)
{
    static int __index = -1;
    st_proto *pro = (st_proto *)arg;
    
    splayer_status_t *status = NULL;

    cJSON *root = NULL;
    cJSON *profile = NULL;

    if(pro == NULL) {
        status = (splayer_status_t *)malloc(sizeof(splayer_status_t));
        REQUIRE(splayer_get_status(status) < 0, Error);
    } else {
        status = &pro->play_status;
    }

    REQUIRE((root = cJSON_CreateObject()) == NULL, Error);
    REQUIRE((profile = cJSON_CreateObject()) == NULL, Error);
    cJSON_AddBoolToObject(root, "status", 1);
    if(force || __index != status->playlist_index) {
        cJSON_AddNumberToObject(root, "musId", status->playlist_index);
        __index = status->playlist_index;
    }
    cJSON_AddNumberToObject(root, "error", 50000);
    cJSON_AddItemToObject(root, "profile", profile);
    int val = 0;
    switch(status->status) {
        case SPLAYER_STATUS_PLAYING: {
            val = 1;
            break;
        }
        case SPLAYER_STATUS_PAUSE: {
            val = 0;
            break;
        }
        default: {
            val = 2;
            break;
        }
    }
    cJSON_AddNumberToObject(profile, "status", val);
    cJSON_AddNumberToObject(profile, "play", status->playlist_index);
    cJSON_AddNumberToObject(profile, "skip", status->current_time);
    cJSON_AddNumberToObject(profile, "setvol", (status->volume * 5) >> 1);

    if(check_is_alarm()) {
        cJSON_AddNumberToObject(profile, "cata", 1);
        splayer_info_t info;
        splayer_get_info(&info);
        if(*info.title) {
            cJSON_AddStringToObject(profile, "bell", info.title);
        } else {
            char  title[256] = "";
            get_music_name(info.uri, title, sizeof(title)-1);
            cJSON_AddStringToObject(profile, "bell", title);
        }
    }

    switch(status->loop_mode) {
        case SPLAYER_LOOP_SHUFFLE: {
            val = 1;
            break;
        }
        case SPLAYER_LOOP_REPEAT_ALL: {
            val = 2;
            break;
        }
        case SPLAYER_LOOP_REPEAT_ONE: {
            val = 3;
            break;
        }
        case SPLAYER_LOOP_SEQUENCE: {
            val = 4;
            break;
        }
        default: {
            val = -1;
            break;
        }
    }
    if(val >= 0) {
        cJSON_AddNumberToObject(profile, "model", val);
    }

    #if 1
    cJSON_AddNumberToObject(profile, "locale", 0);
    #else
    if(status->mode == SPLAYER_MODE_AIMUSIC) {
        val = 1;
    } else {
        val = 0;
    }
    cJSON_AddNumberToObject(profile, "locale", val);
    #endif
    return root;
Error:
    if(root !=  NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    return NULL;
 
}

int tcp_cli_update_status(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_cli("update status");
    cJSON *root = NULL;
    char *out = NULL;
    REQUIRE((root = _json_of_status(pro, 0)) == NULL, Error);
    REQUIRE((out = cJSON_PrintUnformatted(root)) == NULL, Error);
    print_cli("%s", out);
    REQUIRE(cloud_send(pro, C_DEV_CTRL, out) < 0, Error);

    cJSON_Delete(root);
    root = NULL;
    free(out);
    out = NULL;
    return 0;
Error:
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

int hope_update_status(int force)
{
    print_cli("update status msgs");
    int size;
    char *buf = NULL;
    cJSON *root = NULL;
    char *out = NULL;
    st_pro_msg msg;
    msg.type = C_DEV_CTRL;
    REQUIRE((root =_json_of_status(NULL, force)) == NULL, Error);
    REQUIRE((out = cJSON_PrintUnformatted(root)) == NULL, Error);

    print_cli("status update:\n %s", out);

    buf = _build_payload(C_DEV_CTRL, out, &size);
    memcpy(msg.content, buf, size);
    msg.length = size;
    add_msg(halo_handle->msgs, "tcp_cli", msg);
    free(buf);
    buf = NULL;
    free(out);
    out = NULL;
    cJSON_Delete(root);
    root = NULL;
    return 0;
Error:
    if(root != NULL) {
        cJSON_Delete(root);
        root = NULL;
    }
    if(out != NULL) {
        free(out);
        out =NULL;
    }
    return -1;
}
#endif
