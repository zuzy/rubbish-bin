#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <uv.h>

uv_loop_t *loop;
uv_async_t async;
uv_barrier_t barrier;

typedef struct botan_t{
    uv_work_t req;
    int size;
    int index;
    double percentage;
} botan_t;

double percentage;

void fake_download(uv_work_t *req) {
    
    botan_t *tmp = (botan_t *)req->data;

    int size = tmp->size;
    int downloaded = 0;
    while (downloaded < size) {
        // percentage = downloaded*100.0/size;
        tmp->percentage = downloaded * 100.0 / size;
        // printf("%.2f%%\n", tmp->percentage);
        async.data = (void*) tmp;
        uv_async_send(&async);

        sleep(1);
        downloaded += (200+random())%1000; // can only download max 1000bytes/sec,
                                           // but at least a 200;
        // printf("persentage %d\n", downloaded);
    }
}

static int index = 0;
void after(uv_work_t *req, int status) {

    botan_t *tmp = (botan_t *)req->data;

    fprintf(stderr, "[%d] Download complete\n", tmp->index);
    --index;
    if(!index)
        uv_close((uv_handle_t*) &async, NULL);
    free(tmp);
}

void print_progress(uv_async_t *handle) {

    botan_t *tmp = (botan_t *)handle->data;

    // double percentage = *((double*) handle->data);
    // fprintf(stderr, "Downloaded %.2f%%\n", percentage);
    fprintf(stderr, "Downloaded [%d] %.2f%%\n", tmp->index, tmp->percentage);
}

int main() {
    loop = uv_default_loop();
    // uv_work_t req[3];
    // int size = 10240;
    // req.data = (void*) &size;

    uv_async_init(loop, &async, print_progress);
    int i;
    for(i = 0; i < 3; i++) {
        botan_t *tmp = malloc(sizeof(botan_t));
        tmp->size = 10240;
        tmp->index = i;
        tmp->req.data = (void *)tmp;
        uv_queue_work(loop, &tmp->req, fake_download, after);
        ++index;
    }


    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}
