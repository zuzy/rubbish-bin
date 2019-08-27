Pipe 管道
---
| 朱彤 | 2019-8-27
---| ---|

## 目录
* [管道特点](#管道特点)
* [管道读写规则](#管道读写规则)
* [管道的局限性](#管道的局限性)
---

## 管道特点
* 管道是`半双工`的，数据只能向一个方向流动；需要双方通信时，需要建立起两个管道；
* 只能用于父子进程或者兄弟进程之间（具有`亲缘关系的进程`）；
* 单独构成一种独立的文件系统：管道对于管道两端的进程而言，就是一个文件，但它不是普通的文件，它不属于某种文件系统，而是自立门户，`单独构成一种文件系统，并且只存在与内存中`。
* 数据的读出和写入：一个进程向管道中写的内容被管道另一端的进程读出。写入的内容每次都添加在管道缓冲区的末尾，并且每次都是从缓冲区的头部读出数据。
---

## 管道读写规则

管道两端可分别用描述字fd[0]以及fd[1]来描述，需要注意的是，`管道的两端是固定了任务的`。即一端只能用于读，由描述字fd[0]表示，称其为管道读端；另一端则只能用于写，由描述字fd[1]来表示，称其为管道写端。如果试图从管道写端读取数据，或者向管道读端写入数据都将导致错误发生。一般文件的I/O函数都可以用于管道，如close、read、write等等。

从管道中读取数据：

* 如果管道的写端不存在，则认为已经读到了数据的末尾，读函数返回的读出字节数为0；
* 当管道的写端存在时，如果请求的字节数目大于PIPE_BUF，则返回管道中现有的数据字节数，如果请求的字节数目不大于PIPE_BUF，则返回管道中现有数据字节数（此时，管道中数据量小于请求的数据量）；或者返回请求的字节数（此时，管道中数据量不小于请求的数据量）。注：（PIPE_BUF在include/linux/limits.h中定义，不同的内核版本可能会有所不同。Posix.1要求PIPE_BUF至少为512字节，red hat 7.2中为4096）。

Example:
```c
/**************
 * readtest.c *
 **************/
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    int pipe_fd[2];
    pid_t pid;
    char r_buf[100];
    char w_buf[4];
    char *p_wbuf;
    int r_num;
    int cmd;

    memset(r_buf, 0, sizeof(r_buf));
    memset(w_buf, 0, sizeof(r_buf));
    p_wbuf = w_buf;
    if (pipe(pipe_fd) < 0)
    {
        printf("pipe create error\n");
        return -1;
    }

    if ((pid = fork()) == 0)
    {
        printf("\n");
        close(pipe_fd[1]);
        // sleep(3); //确保父进程关闭写端
        r_num = read(pipe_fd[0], r_buf, 100);
        printf("read num is %d   the data read from the pipe is %d\n", r_num, atoi(r_buf));

        close(pipe_fd[0]);
        exit(1);
    }
    else if (pid > 0)
    {
        close(pipe_fd[0]); //read
        strcpy(w_buf, "111");
        if (write(pipe_fd[1], w_buf, 4) != -1)
            printf("parent write over\n");
        close(pipe_fd[1]); //write
        printf("parent close fd[1] over\n");
        // sleep(1);`
    }
}
```

向管道中写入数据：

* 向管道中写入数据时，`linux将不保证写入的原子性`，管道缓冲区一有空闲区域，写进程就会试图向管道写入数据。`如果读进程不读走管道缓冲区中的数据，那么写操作将一直阻塞`。 
* 注：只有在管道的读端存在时，向管道中写入数据才有意义。否则，向管道中写入数据的进程将收到内核传来的SIFPIPE信号，应用程序可以处理该信号，也可以忽略（默认动作则是应用程序终止）。

```c
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int pipe_fd[2];
    pid_t pid;
    char r_buf[4];
    char *w_buf;
    int writenum;
    int cmd;

    memset(r_buf, 0, sizeof(r_buf));
    if (pipe(pipe_fd) < 0)
    {
        printf("pipe create error\n");
        return -1;
    }

    if ((pid = fork()) == 0)
    {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        sleep(10);
        exit(1);
    }
    else if (pid > 0)
    {
        sleep(1);          //等待子进程完成关闭读端的操作
        close(pipe_fd[0]); //write
        w_buf = "111";
        if ((writenum = write(pipe_fd[1], w_buf, 4)) == -1)
            printf("write to pipe error\n");
        else
            printf("the bytes write to pipe is %d \n", writenum);

        close(pipe_fd[1]);
    }
}
```
亲缘关系的进程通信
```c
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int handle_cmd(int cmd);

int main(int argc, char *argv[])
{
    int pipe_fd[2];
    pid_t pid;
    char r_buf[4];
    char **w_buf[256];
    int childexit = 0;
    int i;
    int cmd;

    memset(r_buf, 0, sizeof(r_buf));
    if (pipe(pipe_fd) < 0)
    {
        printf("pipe create error\n");
        return -1;
    }
    if ((pid = fork()) == 0)
    //子进程：解析从管道中获取的命令，并作相应的处理
    {
        printf("\n");
        close(pipe_fd[1]);
        sleep(2);

        while (!childexit)
        {
            read(pipe_fd[0], r_buf, 4);
            cmd = atoi(r_buf);
            if (cmd == 0)
            {
                printf("child: receive command from parent over\n now child process exit\n");
                childexit = 1;
            }

            else if (handle_cmd(cmd) != 0)
                return 0;
            // sleep(1);
        }
        close(pipe_fd[0]);
        exit(0);
    }
    else if (pid > 0)
    //parent: send commands to child
    {
        close(pipe_fd[0]);
        w_buf[0] = "003";
        w_buf[1] = "005";
        w_buf[2] = "777";
        w_buf[3] = "000";
        for (i = 0; i < 4; i++)
            write(pipe_fd[1], w_buf[i], 4);
        close(pipe_fd[1]);
    }
}
//下面是子进程的命令处理函数（特定于应用）：
int handle_cmd(int cmd)
{
    if ((cmd < 0) || (cmd > 256))
    //suppose child only support 256 commands
    {
        printf("child: invalid command \n");
        return -1;
    }
    printf("child: the cmd from parent is %d\n", cmd);
    return 0;
}
```
---
## 管道的局限性
管道的主要局限性正体现在它的特点上：

* 只支持`单向`数据流；
* 只能用于具有`亲缘关系的进程`之间；
* 没有名字；
* 管道的`缓冲区是有限`的（管道制存在于内存中，在管道创建时，为缓冲区分配`一个页面`大小）；
* 管道所传送的是`无格式`字节流，这就要求管道的读出方和写入方必须事先约定好数据的格式，比如多少字节算作一个消息（或命令、或记录）等等；
