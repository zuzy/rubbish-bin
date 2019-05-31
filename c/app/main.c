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
#include "net/config.h"
#include "net/wifi_manager.h"

#include "common.h"

#include "halo_udp.h"
#include "halo_tcp_srv.h"
#include "halo_tcp_cli.h"
#include "halo_uart.h"
#include "halo_http.h"
#include "halo_http_regist.h"

#define HALO_DEBUG 1
#if HALO_DEBUG == 1
    #define print_halo(format, arg...)   do { printf("\033[35m[halo]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_halo(format, arg...)   do {} while (0)
#endif

static enum_error __check_wifi_connection()
{
    wifi_status_e wifi_status;
    hotplug_get_wifi_status(&wifi_status);
    if(wifi_status != WIFI_STATE_CONNECTED){
        print_halo("wait for wifi connection");
        sleep(1);
        return ERR_FORCE_INIT;
    }
    return ERR_IGN;
}

enum_error timeout_cb()
{
    print_halo("time out !!!");
    // check out if wifi connected
    return __check_wifi_connection();
}

static int initcb(void *arg)
{
    print_halo("init 1; wait for wifi connected!");
    return __check_wifi_connection();
}

static int resultcb(void *arg)
{
    st_result *result = (st_result *)arg;
    print_halo("get result %d: %s", result->code, result->body);
    return 0;
}

static int initcb2(void *arg)
{
    // struct list_head posts = LIST_HEAD_INIT(posts);
    print_halo("init 2; start udp tcp-srv uart\n\tbegin to regist");
    // halo_loop_add(halo_init_udp(), halo_udp_recv_cb, 1024, halo_udp_close_cb);
    // halo_loop_add(init_tcp_server(19900), tcp_accept_cb, 1024, tcp_srv_close_cb);
    // halo_loop_add(halo_init_uart(), halo_uart_recv, 1, halo_uart_close);

    // struct list_head bodies = LIST_HEAD_INIT(bodies);
    // add_hash(&bodies, "ver", "val1");
    // add_hash(&bodies, "des", "值");
    // add_hash(&bodies, "cid", "撒的方式的方式而");
    // add_hash(&bodies, "sid", "撒的方式的方式而");
    // add_hash(&bodies, "key", "撒的方式的方式而");
    // add_hash(&bodies, "len", "撒的方式的方式而");
    // add_hash(&bodies, "dat", "撒的方式的方式而");
    // add_post_hash_bodies(&posts, "http://192.168.2.9:8080/hopeApi/upgrade/mac", NULL, &bodies, resultcb);
    // init_http(&posts, 3, 0);
    // halo_http_start();
    // // sleep(5);
    // int re = halo_http_result();
    // deinit_http();

    return regist_first();
    
    // return 0;
}

static int initcb3(void *arg)
{
    print_halo("init3 !!!!!!");
    // halo_loop_add(init_tcp_cli(), tcp_cli_recv_cb, 1024, tcp_cli_close);
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
    halo_common_init(initcb, timeout_cb, 2, 0);
    halo_init_add(initcb, NULL);
    halo_init_add(initcb2, NULL);
    halo_init_add(initcb3, NULL);
    // halo_loop_add(1, loop_cb, 0, 10, NULL);

    halo_common_start();
    print_halo("helo");
    st_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));

    while(1) {
        halo_update(cmd);
        sleep(3);
        // cmd.cmd++;
        // break;
    }
    sleep(1);
    // return 0;
    halo_common_stop();
    halo_common_deinit();
    return 0;
}
