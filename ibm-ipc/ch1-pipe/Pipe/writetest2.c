#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char **argv)
{
    int pipe_fd[2];
    pid_t pid;
    char r_buf[4096];
    char w_buf[4096 * 2];
    int writenum;
    int rnum;
    memset(r_buf, 0, sizeof(r_buf));
    if (pipe(pipe_fd) < 0)
    {
        printf("pipe create error\n");
        return -1;
    }

    if ((pid = fork()) == 0)
    {
        close(pipe_fd[1]);
        while (1)
        {
            sleep(1);
            rnum = read(pipe_fd[0], r_buf, 1000);
            printf("child: readnum is %d\n", rnum);
        }
        close(pipe_fd[0]);

        exit(1);
    }
    else if (pid > 0)
    {
        close(pipe_fd[0]); //write
        memset(r_buf, 0, sizeof(r_buf));
        if ((writenum = write(pipe_fd[1], w_buf, 1024)) == -1)
            printf("write to pipe error\n");
        else
            printf("the bytes write to pipe is %d \n", writenum);
        writenum = write(pipe_fd[1], w_buf, 4096);
        close(pipe_fd[1]);
    }
}
/*
输出结果：
the bytes write to pipe 1000
the bytes write to pipe 1000  //注意，此行输出说明了写入的非原子性
the bytes write to pipe 1000
the bytes write to pipe 1000
the bytes write to pipe 1000
the bytes write to pipe 120  //注意，此行输出说明了写入的非原子性
the bytes write to pipe 0
the bytes write to pipe 0
......
*/