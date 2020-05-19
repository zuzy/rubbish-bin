
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <unistd.h>
#include "erpc_log.h"

#if 1
#define ERPC_LOG_DEBUG(fmt, ...)  printf(fmt"\n", ## __VA_ARGS__)
#else
#define ERPC_LOG_DEBUG(fmt, ...)
#endif

typedef int (*print_cb)(FILE *,const char *, va_list);

static struct log_handle_t {
    FILE *global_fd;
    print_cb pcb;
    int sock;
    struct sockaddr_in addr;
} log_handle = {
    .global_fd = stdout,
    .pcb = vfprintf,
    .sock = -1,
    .addr = {.sin_family = AF_INET},
};

#define log_des \
    _log_xx(file) \
    _log_xx(tcp) \
    _log_xx(udp)

enum log_type{
    #define _log_xx(n) log_e_##n,
    log_des
    #undef _log_xx
};


static int dispatch_file(char *des);
static int dispatch_tcp(char *des);
static int dispatch_udp(char *des);

struct log_types_t{
    log_type t;
    const char *label;
    int (*dis_function)(char *);
} log_types[] = {
    #define _log_xx(n) [log_e_##n]={log_e_##n, #n"://", dispatch_##n},
    log_des
    #undef _log_xx
};


int init_log(const char *des)
{
    #define __num_of__(n) sizeof(n)/sizeof(n[0])
    char *p = (char *)des;
    deinit_log();
    for(int i = 0; i < __num_of__(log_types); ++i) {
        size_t l = strlen(log_types[i].label);
        if((strncmp((char*)des, log_types[i].label, l)) == 0) {
            return log_types[i].dis_function(p + l);
        }
    }
    return -1;
}


static int dispatch_file(char *des)
{
    ERPC_LOG_DEBUG("dispatch_file %s", des);
    FILE *f = fopen(des, "wb");
    int ret = 1;
    if(f) {
        log_handle.global_fd = f;
        ret = 0;
    }
    return ret;
}

static int parse_des(const char *des, struct sockaddr_in *addr)
{
    unsigned int a, b, c, d, port;
    int len = 0;
    if (sscanf(des, "%u.%u.%u.%u:%u%n", &a, &b, &c, &d, &port, &len) == 5) {
        printf("%u %u %u %u %u %d\n", a, b, c, d, port, len);
        /* Bind to a specific IPv4 address, e.g. 192.168.1.5:8080 */
        addr->sin_addr.s_addr =
            htonl(((uint32_t) a << 24) | ((uint32_t) b << 16) | c << 8 | d);
        addr->sin_port = htons((uint16_t) port);
        return 0;
    }
    return 1;
}

static int dispatch_tcp(char *des)
{
    struct sockaddr_in addr = {.sin_family = AF_INET};
    ERPC_LOG_DEBUG("dispatch_tcp %s", des);
    if(parse_des(des, &addr)) {
        return -1;
    }
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == connect(s, (struct sockaddr *)&addr, sizeof(addr))) {
        return -2;
    }
    FILE *f = fdopen(s, "w");
    if(f) {
        log_handle.global_fd = f;
        return 0;
    }
    return -3;
}

static int _udp_print_cb(FILE *f,const char *fmt, va_list arg)
{
    if(log_handle.sock < 0) return -1;
    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt, arg);
    sendto(log_handle.sock, buf, strlen(buf), 0, (struct sockaddr *)&log_handle.addr, sizeof(log_handle.addr));
    return 0;
}

static int dispatch_udp(char *des)
{
    ERPC_LOG_DEBUG("dispatch_udp %s", des);
    if(parse_des(des, &log_handle.addr)) {
        return -1;
    }
    log_handle.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(log_handle.sock < 0) {
        return -2;
    }
    int trueflag = 1;
    setsockopt(log_handle.sock, SOL_SOCKET, SO_BROADCAST, &trueflag, sizeof(trueflag));
    log_handle.pcb = _udp_print_cb;
    
    return 0;
}

void log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_handle.pcb(log_handle.global_fd, fmt, args);
    // vfprintf(global_fd, fmt, args);
    va_end(args);
}

void deinit_log(void)
{
    if(log_handle.global_fd != stdout && log_handle.global_fd != stderr) {
        fclose(log_handle.global_fd);
        log_handle.global_fd = stdout;
    }
    if(log_handle.sock > 0) {
        close(log_handle.sock);
        log_handle.sock = -1;
    }
    log_handle.pcb = vfprintf;
}

#if 1
int main(int argc, char *argv[])
{
    
    ERPC_LOG_DEBUG("tcp_ret %d ", init_log("tcp://127.0.0.1:8888"));
    // ERPC_LOG_DEBUG("udp_ret %d ", init_log("udp://127.0.0.1:8888"));
    // ERPC_LOG_DEBUG("udp_ret %d ", init_log("udp://255.255.255.255:8888"));
    // ERPC_LOG_DEBUG("file_ret:%d", init_log("file://log"));
    log("hello!!!\n");
    // ERPC_LOG_DEBUG("udp_ret:%d", init_log("udp://192.168.1.123:8888"));
    // ERPC_LOG_DEBUG("udp_ret:%d", init_log("udp://192.168.1.123:"));
    deinit_log();
    log("hello!!!\n");
    while(getchar() != EOF);
    return 0;
}

#endif