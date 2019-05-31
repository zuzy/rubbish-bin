#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<string.h>
#include <stdint.h>

#include "list.h"
#include "common.h"
#include "halo_uart.h"

typedef struct _st_uart
{
    int fd;
    int baud;
    char port[128];
    int flowctrl;
    int databits;
    int stopbits;
    int parity;
}st_uart;

static st_uart s_handle = {-1, 9600, "/dev/ttyS4", 0, 8, 1, 'n'};

static int __uart_open(char *port)
{
    int fd;
    fd = open(port, O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd < 0) {
        print_uart("open %s failed", port);
        return -1;
    }
    if(fcntl(fd, F_SETFL, 0) < 0) {
        close(fd);
        print_uart("fcntl failed && close uart");
        return -1;
    }
    print_uart("uart(%s:%d) open ok", port, fd);
    return fd;
}

static int __uart_set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity) 
{
    struct termios options;

    if(tcgetattr(fd, &options) != 0) {
        print_uart("setup uart error");
        goto Error;
    }

    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);
    options.c_cflag |= CLOCAL | CREAD;

    switch(flow_ctrl) {
        case 0: {
            options.c_cflag &= ~CRTSCTS;
            break;
        }
        case 1: {
            options.c_cflag |= CRTSCTS;
            break;
        }
        case 2: {
            options.c_cflag |= IXON | IXOFF | IXANY;
            break;
        }
        default: break;
    }
    options.c_cflag &= ~CSIZE;
    switch(databits) {
        case 5: {
            options.c_cflag |= CS5;
            break;
        }
        case 6: {
            options.c_cflag |= CS6;
            break;
        }
        case 7: {
            options.c_cflag |= CS7;
            break;
        }
        case 8: {
            options.c_cflag |= CS8;
            break;
        }
        default: {
            goto Error;
        }
    }
    switch(parity) {
        case 'n':
        case 'N': {
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        }
        case 'o':
        case 'O': {
            options.c_cflag |= PARODD | PARENB;
            options.c_iflag |= INPCK;
            break;
        }
        case 'e':
        case 'E': {
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;
        }
        case 's':
        case 'S': {
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        }
        default: goto Error;
    }
    switch(stopbits) {
        case 1: {
            options.c_cflag &= ~CSTOPB;
            break;
        }
        case 2: {
            options.c_cflag |= CSTOPB;
            break;
        }
        default: goto Error;
    }

    options.c_oflag &= ~(OPOST | OCRNL);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;

    tcflush(fd, TCIFLUSH);
    if(tcsetattr(fd, TCSANOW, &options) != 0) {
        print_uart("uart set error");
        goto Error;
    }
    return 0;
Error:
    close(fd);
    print_uart("%d closed", fd);
    return -1;
}

int halo_init_uart()
{
    s_handle.fd = __uart_open(s_handle.port);
    if(__uart_set(s_handle.fd, s_handle.baud, s_handle.flowctrl, s_handle.databits, s_handle.stopbits, s_handle.parity)) {
        print_uart("uart init failed");
        return -1;
    }
    return s_handle.fd;
}

int halo_uart_recv(void *arg)
{
    st_loop *loop = (st_loop *)arg;
    memset(loop->body.payload, 0, loop->body.max_len);
    loop->body.len = read(loop->fd, loop->body.payload, 1);
    if(loop->body.len <= 0) {
        if(errno == EINTR) {
            return 0;
        } else {
            print_uart("read failed");
            return -1;
        }
    } else {
        print_uart("recv %d:%s", loop->body.len, loop->body.payload);
        return 0;
    }
}

int halo_uart_close(int fd)
{
    return close(fd);
}