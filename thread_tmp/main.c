#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "cJSON.h"

static void _clean_cb_(void *arg)
{
    char *str = (char *)arg;
    printf("clean up %s\n", str);
    free(str);
}

static void *_proc_(void *arg)
{
    pthread_cleanup_push(_clean_cb_, arg);
    char *str = (char *)arg;
    printf("proc arg %s\n", str);
    // int a = 1/0;
    // while(1) {
    //     sleep(1);
    // }
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]){
    pthread_t pid;
    char *str = calloc(1, 100);
    snprintf(str, 99, "byby %s", "zizy");
    pthread_create(&pid, NULL, _proc_, (void *)str);
    pthread_detach(pid);
    // sleep(5);
    // pthread_kill(pid, 11);
    // pthread_cancel(pid);

    FILE *fp = fopen("tmp", "r");
    if(fp) {
        char tmp[4] = "";
        int ret = fread(tmp, 1, 4, fp);
        printf("get %d --> %s  %d\n", ret, tmp, atoi(tmp));

        fclose(fp);
    }
    

    // free(str);
    while(1) {
        sleep(1);
    }
    return 0;
}