
#ifndef __HALO_UDP_H__
#define __HALO_UDP_H__

#define HALO_UDP_DEBUG 1
#if HALO_UDP_DEBUG == 1
    #define print_udp(format, arg...)   do { printf("\033[31m[halo_udp]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_udp(format, arg...)   do {} while (0)
#endif

int halo_init_udp();
int halo_udp_recv_cb(void *arg);
int halo_udp_close_cb(int fd);

#endif