#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <uv.h>

#define UV_THREADPOOL_SIZE 128

#define FIB_UNTIL 25
uv_loop_t *loop;

typedef struct {
    uv_work_t req;
    int num;
} botan_t;

long fib_(long t) {
    if (t == 0 || t == 1)
        return 1;
    else
        return fib_(t-1) + fib_(t-2);
}

void fib(uv_work_t *req) {
    // int n = *(int *) req->data;
    botan_t *botan = (botan_t *)req->data;
    int n = botan->num;
    
    if (random() % 2)
        sleep(1);
    else
        sleep(3);
    long fib = fib_(n);
    fprintf(stderr, "%dth fibonacci is %lu\n", n, fib);
}

void after_fib(uv_work_t *req, int status) {
    // fprintf(stderr, "Done calculating %dth fibonacci\n", *(int *) req->data);
    botan_t *botan = (botan_t *)req->data;
    fprintf(stderr, "Done calculating %dth fibonacci\n", botan->num);
    free(botan);
}

int main() {
    loop = uv_default_loop();

    int data[FIB_UNTIL];
    uv_work_t req[FIB_UNTIL];
    
    int i;
    for (i = 0; i < FIB_UNTIL; i++) {
        // data[i] = i;
        // req[i].data = (void *) &data[i];
        // uv_queue_work(loop, &req[i], fib, after_fib);
        botan_t *botan = malloc(sizeof(botan_t));
        botan->req.data = (void *)botan;
        botan->num = i;
        uv_queue_work(loop, &botan->req, fib, after_fib);
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}
