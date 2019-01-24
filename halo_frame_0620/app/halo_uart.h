#ifndef __HALO_UART_H__
#define __HALO_UART_H__

#define HALO_UART_DEBUG 1
#if HALO_UART_DEBUG == 1
    #define print_uart(format, arg...)   do { printf("\033[31m[halo_uart]\033[0m:%s,%d--- "format"\n",__FILE__,__LINE__, ## arg);} while (0)
#else
    #define print_uart(format, arg...)   do {} while (0)
#endif

int halo_init_uart();
int halo_uart_recv(void *arg);
int halo_uart_close(int fd);

#endif