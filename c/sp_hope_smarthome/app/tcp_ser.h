#ifndef __TCP_SER_H__
#define __TCP_SER_H__

#define HALO_TCP_SER_DEBUG 0
#if HALO_TCP_SER_DEBUG == 1
    #define print_ser(format, arg...)   do { printf("\033[31m[halo_ser]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_ser(format, arg...)   NULL
#endif

int add_tcp_ser(void);
int tcp_ser_port(void);
#endif