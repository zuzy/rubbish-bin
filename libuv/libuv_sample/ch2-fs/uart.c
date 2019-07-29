#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <uv.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>


void on_read(uv_fs_t *req);

uv_fs_t open_req;
uv_fs_t read_req;
uv_fs_t write_req;
uv_fs_t write_to_req;

static char buffer[1024];

static uv_buf_t iov;

struct _st_serial{
    int fd;
    int send_timer;
    char dev[256];
    int baud;
    int max_size;
} serial_handle = {
    -1,
    -1,
    "/dev/ttyUSB0",
    115200,
    128,
};


static int _serial_open(void)
{
    serial_handle.fd = open(serial_handle.dev, O_RDWR|O_NOCTTY|O_NDELAY);
    fcntl(serial_handle.fd, F_SETFL, 0);
    return 0;
Error:
    if(serial_handle.fd >= 0) {
        close(serial_handle.fd);
        serial_handle.fd = -1;
    }
    return -1;
}

static int _serial_cfg(uv_fs_t *req)
{
    fcntl(req->result, F_SETFL, 0);
    struct termios opt;
    tcgetattr(req->result, &opt);
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

    tcflush(req->result, TCIFLUSH);
    
    tcsetattr(req->result, TCSANOW, &opt);
    printf("open ok\n");
    
    return 0;

Error:
    if(open_req.result >= 0) {
        close(open_req.result);
    }
    return -1;
}



void on_write(uv_fs_t *req) {
    if (req->result < 0) {
        fprintf(stderr, "Write error: %s\n", uv_strerror((int)req->result));
    }
    else {
        uv_fs_read(uv_default_loop(), &read_req, open_req.result, &iov, 1, -1, on_read);
    }
}

void wait_for_a_while(uv_idle_t *handle)
{
    usleep(100000);
}

void on_read(uv_fs_t *req) {
    printf("reading \n");
    if (req->result < 0) {
        fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
    }
    // else if (req->result == 0) {
    //     uv_fs_t close_req;
    //     // synchronous
    //     uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
    // }
    else if (req->result >= 0) {
        iov.len = req->result;
        if(req->result > 0) {
            uv_fs_write(uv_default_loop(), &write_req, write_to_req.result, &iov, 1, -1, on_write);
        } else {
            uv_idle_t idle;
            uv_idle_init(uv_default_loop(), &idle);
            uv_idle_start(&idle,wait_for_a_while);
        }
    }
}

void on_open(uv_fs_t *req) {
    // The request passed to the callback is the same as the one the call setup
    // function was passed.
    // fcntl(req->result, F_SETFL, 0);
    _serial_cfg(req);
    // assert(req == &open_req);
    if (req->result >= 0) {
        iov = uv_buf_init(buffer, sizeof(buffer));
        printf("on read!!\n");
        uv_fs_read(uv_default_loop(), &read_req, req->result,
                   &iov, 1, -1, on_read);
    }
    else {
        fprintf(stderr, "error opening file: %s\n", uv_strerror((int)req->result));
    }
}

int main(int argc, char **argv) {
    uv_fs_open(uv_default_loop(), &open_req, serial_handle.dev, O_RDWR|O_NOCTTY|O_NDELAY, 0, on_open);
    
    // if(argv[2] && strlen(argv[2]))
    // uv_fs_open(uv_default_loop(), &write_to_req, argv[2], O_RDWR | O_CREAT | O_APPEND, 0664, NULL);
    // xelse 
    write_to_req.result = 1;

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    uv_fs_req_cleanup(&open_req);
    uv_fs_req_cleanup(&read_req);
    uv_fs_req_cleanup(&write_req);
    if(write_to_req.result != 1) 
        uv_fs_req_cleanup(&write_to_req);
    return 0;
}
