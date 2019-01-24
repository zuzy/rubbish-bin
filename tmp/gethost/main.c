#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SELECT_HOST 1

#define HALO_COMMON_DEBUG 1
#if HALO_COMMON_DEBUG == 1
    #define print_common(format, arg...)   do { printf("\033[31m[halo_common]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_common(format, arg...)   NULL
#endif

#define print_cli print_common

#define REQUIRE(in,tag)                 do{if(in){print_common("%s %s", #tag, #in); goto tag;}}while(0)

int port = 6666;
// host: open.nbhope.cn:6666
char host[] = "open.nbhope.cn";

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
                print_common("n : %d \terror is %d", n, errno);
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
    return 0;
Exit:
    if(cli_fd >= 0) {
        close(cli_fd);
    }
    cli_fd = -1;
    return -1;
}

int conn_host(char *host, char *port)
{
    struct addrinfo hints;
    struct addrinfo *res, *rp;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    int ret = getaddrinfo(host, port, &hints, &res);
    if(ret != 0 && res != NULL){
        print_cli("error");
        return -1;
    } else {
        print_cli("get info ok");
        int fd = -1;
        for(rp = res; rp != NULL; rp = rp->ai_next) {
            fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if(fd == -1) continue;
            set_nonblocking(fd);
            if(tcp_connect(fd, *(struct sockaddr_in *)rp->ai_addr, 0) == 0) {
                print_cli("conn ok !");
                break;
            }
            close(fd);
        }
        if(rp == NULL) {
            print_cli("cannot connect!");
        }
        while(1) {
            sleep(1);
        }
        freeaddrinfo(res);
        print_cli("close");
        close(fd);
    }
}

int tcp_cli_init(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *he;
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    #if SELECT_HOST == 1
    REQUIRE((he = gethostbyname(host)) == NULL, Error);
    #else
    REQUIRE((he = gethostbyname(host)) == NULL, Error);
    #endif

    server.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
#if 1
    set_nonblocking(fd);
    printf("host: %s:%d\n", host, port);
    // usleep(1000);
    // sleep(1);
    fd = tcp_connect(fd, server, 0);
    
    REQUIRE((fd < 0), Error);

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
    // set_nonblocking(fd);

    print_common("connect ok");
    return 0;
Error:
    if(fd >= 0) {
        close(fd);
    }
    fd = -1;
    return -1;
}

struct _st_ptr {
    int *p1;
    int *p2;
};

#define min(a, b) ({ \
      __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
      (void)(&_a == &_b); \
      _a < _b ? _a : _b; \
})

int main(int argc, char *argv[])
{
    int fd = -1;
    int a = 20;
    // double b = 30;
    char *b = "sdf";
    int s = min(a,b);
    printf("min is %d\n", s);
    while(1) {
        struct _st_ptr *s = calloc(1, sizeof(struct _st_ptr));
        if(s->p1 != NULL)
            printf("null is %p\n", s);
        usleep(100);
    }
    while(1) {
        fd = tcp_cli_init();
        print_cli("fd = %d", fd);
        close(fd);
        conn_host(host, "6666");
        sleep(2);
    }
    return 0;
}