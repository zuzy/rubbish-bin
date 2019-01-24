#ifndef __UDP_SER_H__
#define __UDP_SER_H__

#define HALO_UDP_DEBUG 1
#if HALO_UDP_DEBUG == 1
    #define print_udp(format, arg...)   do { printf("\033[31m[halo_udp]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_udp(format, arg...)   NULL
#endif
int udp_service_init();
#endif