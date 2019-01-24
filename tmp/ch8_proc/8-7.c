#include "apue.h"
static void charatime(char *);
int main()
{
    pid_t pid;
    TELL_WAIT();
    if((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid == 0) {
        WAIT_PARENT();  // WAIT FOR PARENT STOP
        charatime("output from child\n");
    } else {
        charatime("output from parent\n");
        TELL_CHILD();   // TELL CHILD PARENT IS STOP
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