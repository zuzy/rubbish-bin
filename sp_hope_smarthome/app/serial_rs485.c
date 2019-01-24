#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include "applib.h"
#include "common.h"
#include "main_loop.h"
#include "serial_rs485.h"
#include "rs485_dispatch.h"

struct _st_serial{
    int fd;
    int send_timer;
    char dev[256];
    int baud;
    int max_size;
    st_rs485 body;
} serial_handle = {
    -1,
    -1,
    "/dev/ttyS4",
    9600,
    128,
};

static void _serial_dump(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    int i;
    print_serial("serial buf: %d", pro->recv_buf.len);
    for(i = 0; i < pro->recv_buf.len; i++) {
        printf(" %x", pro->recv_buf.payload[i]);
    }
    printf("\n");
}

static _serial_open()
{
    REQUIRE((serial_handle.fd = open(serial_handle.dev, O_RDWR|O_NOCTTY|O_NDELAY)) < 0, Error);
    REQUIRE(fcntl(serial_handle.fd, F_SETFL, 0) < 0, Error);
    return 0;
Error:
    if(serial_handle.fd >= 0) {
        close(serial_handle.fd);
        serial_handle.fd = -1;
    }
    return -1;
}

static int _serial_cfg()
{
    struct termios opt;
    REQUIRE(tcgetattr(serial_handle.fd, &opt) != 0, Error);
    cfsetispeed(&opt, serial_handle.baud);
    cfsetospeed(&opt, serial_handle.baud);
    opt.c_cflag |= CLOCAL | CREAD;
    opt.c_cflag &= ~(CRTSCTS | CSIZE);

    opt.c_cflag |= CS8;
    opt.c_cflag &= ~PARENB;
    opt.c_iflag &= ~INPCK;

    opt.c_cflag &= ~CSTOPB;

    opt.c_oflag &= ~OPOST;
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    opt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    opt.c_cc[VTIME] = 0;
    opt.c_cc[VMIN] = 0;

    tcflush(serial_handle.fd, TCIFLUSH);
    
    REQUIRE(tcsetattr(serial_handle.fd, TCSANOW, &opt) != 0, Error);
    
    return 0;

Error:
    if(serial_handle.fd >= 0) {
        close(serial_handle.fd);
        serial_handle.fd = -1;
    }
    return -1;
}

int serial_init(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    REQUIRE(_serial_open() != 0, Error);
    REQUIRE(_serial_cfg() != 0, Error);
    print_serial("open & config %s -> %d ok", serial_handle.dev, serial_handle.fd);
    pro->fd = serial_handle.fd;
    pro->active = 1;
    return 0;
Error:
    pro->fd = -1;
    pro->active = 0;
    return -1;
}

int serial_close(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    print_serial("close ser");
    if(pro != NULL && pro->fd >= 0) {
        close(pro->fd);
        pro->fd = -1;
        pro->active = 0;
        memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
        pro->recv_buf.len = 0;
        return 0;
    }
    return -1;
}

static int s_head_len = sizeof(st_rs485_head);

int serial_recv(void *arg)
{
    int len = 0;
    st_rs485 *ptr;
    st_proto *pro = (st_proto *)arg;
    len = read(pro->fd, pro->recv_buf.payload + pro->recv_buf.len, 1);
    // print_serial("serial pro %d, %d, %s", pro->recv_buf.begin, pro->recv_buf.len, pro->recv_buf.payload);
    if(len > 0) {
        reset_timer(pro);
    
    #if 0
        pro->recv_buf.len += len;
        _serial_dump(pro);
        if(pro->recv_buf.len >= pro->recv_buf.max_len) {
            memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
            pro->recv_buf.len = 0;
        }
    #else
        if(pro->recv_buf.begin) {
            ++pro->recv_buf.len;
            ptr = (st_rs485 *)pro->recv_buf.payload;
            if(ptr->head.len && pro->recv_buf.len >= (ptr->head.len + s_head_len + 1)) {
                // dis;
                _serial_dump(pro);

                memcpy(&serial_handle.body, ptr, sizeof(st_rs485));
                serial_handle.body.check = *(ptr->body.body + ptr->head.len - 1);
                print_serial("serial %.02x, chk %.02x", serial_handle.body.body.cmd, serial_handle.body.check);
                if(check_addr_pro(pro)) {
                    return rs485_dispatch(pro);
                } else {
                    memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
                    return 1;
                }

                // if(rs485_check(&serial_handle.body) != 0) {
                //     print_serial("check ok");
                // } else {
                //     print_serial("check error!");
                // }

                // memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
                // pro->recv_buf.len = 0;
                // pro->recv_buf.begin = 0;
                // return 0;
            }
        } else {
            if(0xaa == *pro->recv_buf.payload) {
                // memset(&pro->recv_buf.payload, 0, pro->recv_buf.max_len);
                pro->recv_buf.begin = 1;
                ++pro->recv_buf.len;
            } else {
                pro->recv_buf.len = 0;
            }
        }
        // _serial_dump(pro);
    #endif
    }
    return 1;
}

static void _send_enable()
{
    print_serial("send enable");
    system("echo 1 > /sys/class/gpio/gpio103/value ");
}

static void _send_disable()
{
    print_serial("send dis");
    system("echo 0 > /sys/class/gpio/gpio103/value ");
}

static void _recv_init()
{
    _send_disable();
}

static int _send_stop(void *arg)
{
    st_proto *pro = (st_proto *)arg;
    _send_disable();
    tcflush(pro->fd, TCOFLUSH);
    serial_handle.send_timer = -1;
    return 0;
}

static int _serial_send(st_proto *pro)
{
    if(pro->recv_buf.len <= 0) {
        return 0;
    }
    _send_enable();
    write(pro->fd, pro->recv_buf.payload, pro->recv_buf.len);
    if(serial_handle.send_timer < 0)
        serial_handle.send_timer = set_single_shot_timer(pro->recv_buf.len, _send_stop, pro);
    if(serial_handle.send_timer >= 0)
        return 0;
    else
        return -1;
}

int serial_reply(void *arg)
{
    print_serial("serial reply");
    st_proto *pro = (st_proto *)arg;
    if(pro->fd >= 0 && pro->active) {
        _serial_send(pro);
        memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
        pro->recv_buf.len = 0;
        return 0;
    }
    return -1;
}

int serial_act(void *arg)
{
    return 0;
}

int serial_dispro(void *arg)
{
    return 0;
}

int serial_to(void *arg)
{
    // print_serial("serial timeout !!!");
    st_proto *pro = (st_proto *)arg;
    if(pro->recv_buf.len) {
        print_serial("reset serial proto");
        pro->recv_buf.len = 0;
        memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
    }
    // char str[] = "hello world\n";
    // strcpy(pro->recv_buf.payload, str);
    // pro->recv_buf.len = strlen(str);
    // _serial_send(pro);
    // memset(pro->recv_buf.payload, 0, pro->recv_buf.max_len);
    // pro->recv_buf.len = 0;
    return 0;
}

static st_callbacks ser_cbs = {
    serial_init,
    serial_recv,
    serial_reply,
    serial_act,
    serial_dispro,
    serial_to,
    // NULL,
    serial_close,
};

int add_serial()
{
    st_proto *pro = (st_proto *)malloc(sizeof(st_proto));
    REQUIRE(pro == NULL, Error);
    memset(pro, 0, sizeof(st_proto));
    strcpy(pro->name, "rs485");
    pro->config |= LOOP_CFG_RETRY;
    pro->recv_buf.payload = (char *)malloc(serial_handle.max_size);
    pro->recv_buf.begin = 0;
    pro->recv_buf.len = 0;
    pro->recv_buf.max_len = serial_handle.max_size;
    pro->callbacks = &ser_cbs;
    pro->timeout = (1 << 10);
    print_serial("add %s", pro->name);
    init_485_addr();
    return add_proto(halo_handle, pro);
Error:
    // sdfsdf;
    return -1;
}