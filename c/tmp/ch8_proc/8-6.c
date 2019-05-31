#include "apue.h"
static void charatime(char *);
int main()
{
    pid_t pid;
    if((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        charatime("output from child\n");
    } else {
        charatime("output from parent\n");
    }
    exit(0);
}
static void charatime(char *str)
{
    char *ptr;
    int c;
    setbuf(stdout, NULL);
    for(ptr = str; (c = *ptr++)!= 0; )
        putc(c, stdout);
}

