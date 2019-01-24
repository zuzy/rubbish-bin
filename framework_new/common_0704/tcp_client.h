#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#define HALO_TCP_CLI_DEBUG 1
#if HALO_TCP_CLI_DEBUG == 1
    #define print_cli(format, arg...)   do { printf("\033[31m[halo_cli]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_cli(format, arg...)   NULL
#endif

int add_tcp_cli();

#endif