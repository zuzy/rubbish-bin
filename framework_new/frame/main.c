#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>
#include "list.h"

#include "common.h"

#include "halo_udp.h"
#include "halo_tcp_srv.h"
#include "halo_tcp_cli.h"
#include "halo_uart.h"

#define HALO_DEBUG 1
#if HALO_DEBUG == 1
    #define print_halo(format, arg...)   do { printf("\033[35m[halo]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_halo(format, arg...)   do {} while (0)
#endif

enum_error timeout_cb()
{
    print_halo("time out !!!");
    // check out if wifi connected

    return ERR_IGN;
}

static int initcb(void *arg)
{
    static int i = 0;
    print_halo("init !!!!!!");
    sleep(1);
    if(i++ < 10)
        return ERR_FORCE_INIT;
    return 0;
}

static int initcb2(void *arg)
{
    print_halo("init2 !!!!!!");
    return 0;
}
static int initcb3(void *arg)
{
    print_halo("init3 !!!!!!");
    halo_loop_add(halo_init_udp(), halo_udp_recv_cb, 1024, halo_udp_close_cb);
    halo_loop_add(init_tcp_server(19900), tcp_accept_cb, 1024, tcp_srv_close_cb);
    halo_loop_add(init_tcp_cli(), tcp_cli_recv_cb, 1024, tcp_cli_close);
    // halo_loop_add(halo_init_uart(), halo_uart_recv, 1, halo_uart_close);
    return 0;
}
static st_cmd loop_cb(void *arg)
{
    st_cmd cmd;
    return cmd;
}

void * test_proc(void *arg)
{
    st_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));
    while(1) {
        halo_update(cmd);
        sleep(3);
        cmd.cmd++;
    }
}


int main()
{
    // int i[2];
    // print_halo("%d, %d", i[0], i[1]);
    // memset(i, 0, 2 * sizeof(int));
    // if(pipe(i) == -1) {
    //     print_halo();
    // }
    // print_halo("%d, %d", i[0], i[1]);
    // halo_common_init(NULL, NULL, 10, 0);
    halo_common_init(NULL, timeout_cb, 2, 0);
    halo_init_add(initcb, NULL);
    halo_init_add(initcb2, NULL);
    halo_init_add(initcb3, NULL);
    // halo_loop_add(1, loop_cb, 0, 10, NULL);

    halo_common_start();
    print_halo("helo");
    st_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));

    while(1) {
        // halo_update(cmd);
        // sleep(3);
        // cmd.cmd++;
        // break;
    }
    sleep(1);
    // return 0;
    halo_common_stop();
    halo_common_deinit();
    return 0;
}
