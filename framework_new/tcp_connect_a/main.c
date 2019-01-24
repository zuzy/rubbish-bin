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

#include <netdb.h>

#define HALO_COMMON_DEBUG 1
#if HALO_COMMON_DEBUG == 1
    #define print_common(format, arg...)   do { printf("\033[31m[halo_common]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_common(format, arg...)   NULL
#endif
#define REQUIRE(in,tag)                 do{if(in){print_common("err! %s", #in); goto tag;}}while(0)

typedef struct _st_proto
{
    int fd;
}st_proto;


struct _st_villa
{
    int init_tcp;
    int udp_port;
    int udp_from_port;
    char tcp_host[64];
    int tcp_port;
    int max_buf_len;
    struct sockaddr_in udp_from;
    struct sockaddr_in udp_bro;
} villa_handle = {
    1,
    6666,
    7777,
    "",
    8888,
    1024,
};

char *buffer[1024];

void set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        print_common("fcntl(F_GETFL)");
    }
    if ((flags & O_NONBLOCK) == O_NONBLOCK) {
        return;
    }
    flags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (flags == -1) {
        print_common("fcntl(F_SETFL)");
    }
}

static int _connect(int *cli_fd, 
            struct sockaddr_in server_addr, 
            int init)
{
    print_common("fd is %d %d", *cli_fd, init);
    int n = 0;
    int err;
    socklen_t len;
    if(init) {
        int flags = fcntl(*cli_fd,F_GETFL,0);
        fcntl(*cli_fd,F_SETFL,flags | O_NONBLOCK);

        // n = connect(*cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    //     if(n < 0) {
    //         REQUIRE((errno != EINPROGRESS && errno != EWOULDBLOCK), Exit);
    //         return 1;
    //     } else {
    //         *cli_fd = n;
    //         return 0;
    //     }
    // } else {
        while(1) {
            connect(*cli_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            fd_set wset;
            fd_set eset;
            FD_ZERO(&wset);
            FD_SET(*cli_fd,&wset);
            n = select((*cli_fd)+1,NULL,&wset,NULL,&tv);
            print_common("select %d", n);
            if(n < 0) {
                // REQUIRE((errno != EINPROGRESS && errno != EWOULDBLOCK), Exit);
                if(errno != EINPROGRESS && errno != EWOULDBLOCK) {
                    return 1;
                }
            } else if(n == 0) {
                continue;
            } else {
                if(FD_ISSET(*cli_fd, &wset)) {
                    // REQUIRE((getsockopt(*cli_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0), Exit);
                    // REQUIRE(err != 0, Exit);
                    if((getsockopt(*cli_fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0) || err != 0) {
                        print_common("fd %d is set, error %d", *cli_fd, err);
                        continue;
                    }
                        
                    return 0;
                }
            }
        }
        return 1;            
    }
Exit:
    // if(*cli_fd >= 0) {
    //     close(*cli_fd);
    // }
    // *cli_fd = -1;
    return -1;
}

static int _villa_tcp_init_async(st_proto *pro)
{
    static struct sockaddr_in server;
    if(villa_handle.init_tcp) {
        REQUIRE( (pro->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0, Error);
        struct hostent *he;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(villa_handle.tcp_port);
        he = gethostbyname(villa_handle.tcp_host);
        server.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
        set_nonblocking(pro->fd);
        print_common("init fd ok %d", pro->fd);
        
        int init = _connect(&pro->fd, server, villa_handle.init_tcp);
        // int init = connect(pro->fd, (struct sockaddr *)&server, sizeof(server));
        REQUIRE(init < 0, Error);
        villa_handle.init_tcp = !init;
        print_common("to connect %d", villa_handle.init_tcp);
        return init;
    } else {
        print_common();
        int init = _connect(&pro->fd, server, villa_handle.init_tcp);
        print_common("init is %d", init);
        REQUIRE(init < 0, Error);
        villa_handle.init_tcp = !init;
        print_common("to connect %d %d", villa_handle.init_tcp, init);
        return init;
    }
Error:
    NULL;
    return -1;
}

static int init_connection(st_proto *pro)
{
    static struct sockaddr_in server;
    REQUIRE( (pro->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0, Error);
    struct hostent *he;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(villa_handle.tcp_port);
    he = gethostbyname(villa_handle.tcp_host);
    server.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    set_nonblocking(pro->fd);
    print_common("init fd ok %d", pro->fd);
    connect(pro->fd, (struct sockaddr *)&server, sizeof(server));
    return 0;
Error:
    NULL;
    return -1;
}

static int check_connection(st_proto *pro)
{
    struct timeval tv;
    int err;
    socklen_t len;
    int n;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    fd_set wset;
    fd_set eset;
    FD_ZERO(&wset);
    FD_SET(pro->fd,&wset);
    print_common("%d", pro->fd);
    n = select(pro->fd+1,NULL,&wset,NULL,&tv);
    print_common("select %d", n);
    if(n < 0) {
        REQUIRE((errno != EINPROGRESS && errno != EWOULDBLOCK), Exit);
        return -1;
    } else if(n == 0) {
        return 1;
    } else {
        if(FD_ISSET(pro->fd, &wset)) {
            REQUIRE((getsockopt(pro->fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0), Exit);
            REQUIRE(err != 0, Exit);
            if(err != 0) {
                return 1;
            }
            print_common("fd %d is set, error %d", pro->fd, err);
            return 0;
        }
    }
Exit:
    return 1;
}

char *to_str = "{\"type\":\"REQUEST_TCP\",\"sn\":\"hi-2017-04-26\"}";
static int _villa_udp_to(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    char out[] = "asdfghjkl;\n";
    socklen_t len = sizeof(struct sockaddr_in);

    sendto(pro->fd, to_str, strlen(to_str), 0, (struct sockaddr *)&villa_handle.udp_bro, len);
    print_common("send to ...");
    return 0;
Error:
    return -1;
}

static int _villa_udp_recv(st_proto *pro)
{
    struct sockaddr_in from;
    memset(buffer, 0, 1024);
    socklen_t len = sizeof(struct sockaddr_in);
    int length = recvfrom(pro->fd, buffer, 1024, 0, (struct sockaddr *)&from, &len);
    print_common("recv %d, %s", length, buffer);
}

static int _villa_udp_init(st_proto *pro)
{
    int opt = 1;
    struct sockaddr_in server_addr;
    struct hostent *he = gethostbyname("192.168.31.255");


    REQUIRE(((pro->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0), Error);

    memset(&villa_handle.udp_from, 0, sizeof(villa_handle.udp_from));
    villa_handle.udp_from.sin_family = AF_INET;
    villa_handle.udp_from.sin_port = htons(villa_handle.udp_from_port);
    villa_handle.udp_from.sin_addr.s_addr = htonl(INADDR_ANY);
    // villa_handle.udp_from.sin_addr = *((struct in_addr *)he->h_addr);

    memset(&villa_handle.udp_bro, 0, sizeof(villa_handle.udp_bro));
    villa_handle.udp_bro.sin_family = AF_INET;
    villa_handle.udp_bro.sin_port = htons(villa_handle.udp_port);
    villa_handle.udp_bro.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    REQUIRE(setsockopt(pro->fd, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)) < 0, Error);
    REQUIRE((bind(pro->fd, (struct sockaddr *)&(villa_handle.udp_from), sizeof(villa_handle.udp_from)) < 0), Error);
    return 0;
Error:
    NULL;
    return -1;
}

char *jsonbuf = "{\"type\":\"RESPONSE_TCP\",\"version\":\"2.0.0.1\",\"data\":{\"ip\":\"192.168.31.246\",\"port\":8888,\"company\":\"VillaKit_16\"}";

int main()
{
    struct sockaddr_in from_addr;
    int ret = 0;
    st_proto pro;
    print_common("init tcp");
    printf("connecting");
    // _villa_tcp_init_async(&pro);
    _villa_udp_init(&pro);
    // init_connection(&pro);
        // while(1);
    // ret = check_connection(&pro);
    while(1) {
        printf(".");
        // ret = check_connection(&pro);
        // usleep(10);
        if(ret == 0) {
            break;
        }
    }
    printf("\nconnect ok! %d\n",pro.fd);

    char buf[1024] = "";
    int length = 0;
    int from_len = sizeof(struct sockaddr_in);
    while(1) {
        sleep(1);
        _villa_udp_to(&pro);
        usleep(100000);
        _villa_udp_recv(&pro);
        // _villa_udp_recv(&pro);
        // memset(buf, 0, 1024);
        // length = recvfrom(pro.fd, buf, 1024, 0, (struct sockaddr *)&villa_handle.udp_from, sizeof(struct sockaddr_in));
        // print_common("recv %d %s", length, buf);
        
        // length = recvfrom(pro.fd, buf, 1024, 0, (struct sockaddr *)&from_addr, &from_len);
        // print_common("recv %d %s", length, buf);

        // sendto(pro.fd, jsonbuf, strlen(jsonbuf), 0, (struct sockaddr *) &villa_handle.udp_bro, sizeof(villa_handle.udp_bro));

    }
}