#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define NLOOPS      1000
#define SIZE        sizeof(long)

static int update(long *ptr)
{
    return (*ptr)++;
}

int main(int argc, char *argv[])
{
    int fd, i, counter;
    pid_t pid;
    void *area;

    if((fd = open("/dev/zero", O_RDWR)) < 0) {
        fprintf(stderr, "open /dev/zero error\n");
    }
    if((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        fprintf(stderr, "mmap error\n");
    }
    close(fd);

    TELL_WAIT();

    if((pid = fork()) < 0) {
        fprintf(stderr, "fork error\n");
    } else if(pid > 0) {
        for(i = 0; i < NLOOPS; i+=2) {
            if((counter = update((long *)area)) != i) {
                fprintf(stderr, "parent: expected %d got %d\n", i, counter);
            }
            TELL_CHILD(pid);
            
        }
    }
    return 0;
}