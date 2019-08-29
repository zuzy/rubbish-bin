#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// #define FIFO_SERVER "/tmp/fifoserver"
#define FIFO_SERVER "/tmp/echo.sock"
int main(int argc, char **argv)
//参数为即将写入的字节数
{
    int fd;
    char w_buf[4096 * 2];
    int real_wnum;
    memset(w_buf, 0, 4096 * 2);
    if ((mkfifo(FIFO_SERVER, O_CREAT | O_EXCL) < 0) && (errno != EEXIST))
        printf("cannot create fifoserver\n");
    if (fd == -1)
        if (errno == ENXIO)
            printf("open error; no reading process\n");
    // fd = open(FIFO_SERVER, O_WRONLY | O_NONBLOCK, 0);
    //设置非阻塞标志
    fd=open(FIFO_SERVER,O_WRONLY,0);
    //设置阻塞标志

    // real_wnum = write(fd, w_buf, 2048);
    real_wnum = write(fd, argv[0], strlen(argv[0]));
    if (real_wnum == -1)
    {
        if (errno == EAGAIN)
            printf("write to fifo error; try later\n");
    }
    else
        printf("real write num is %d\n", real_wnum);
    real_wnum = write(fd, w_buf, 5000);
    //5000用于测试写入字节大于4096时的非原子性
    //real_wnum=write(fd,w_buf,4096);
    //4096用于测试写入字节不大于4096时的原子性
    if (real_wnum == -1)
        if (errno == EAGAIN)
            printf("try later\n");
    return 0;
}