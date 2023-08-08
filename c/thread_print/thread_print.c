#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>

static void *_proc_(void *arg)
{
    size_t len = 10;
    int id = *(int*)arg;
    char *s = NULL;
    while(1) {
        s = (char*)realloc(s, len);
        memset(s, id + '0', len - 1);
        s[len - 1] = '\0';
        printf("%s\n", s);
        len += 10;
        usleep(1000);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    int i;
    for(i = 0; i < 10; ++i) {
        pthread_t pid;
        pthread_create(&pid, NULL, _proc_, (void *)&i);
        pthread_detach(pid);

    }
    while(1) {
        sleep(1);
    }
    return 0;
}