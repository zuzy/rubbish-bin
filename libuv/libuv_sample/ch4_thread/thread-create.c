#include <stdio.h>
#include <unistd.h>

#include <uv.h>

#include <pthread.h>

uv_barrier_t blocker;

void hare(void *arg) {
    pthread_detach(pthread_self());
    int tracklen = *((int *) arg);
    while (tracklen) {
        tracklen--;
        sleep(1);
        fprintf(stderr, "Hare ran another step\n");
    }
    fprintf(stderr, "Hare done running!\n");
    // uv_barrier_wait(&blocker);
    if (uv_barrier_wait(&blocker) > 0) {
        printf("?\n");
        uv_barrier_destroy(&blocker);
    }
}

void tortoise(void *arg) {
    pthread_detach(pthread_self());
    int tracklen = *((int *) arg);
    while (tracklen) {
        tracklen--;
        fprintf(stderr, "Tortoise ran another step\n");
        sleep(3);
    }
    fprintf(stderr, "Tortoise done running!\n");
    uv_barrier_wait(&blocker);
}

int main() {
    int tracklen = 3;
    uv_thread_t hare_id;
    uv_thread_t tortoise_id;
    uv_barrier_init(&blocker, 3);
    uv_thread_create(&hare_id, hare, &tracklen);
    
    uv_thread_create(&tortoise_id, tortoise, &tracklen);

    // uv_thread_join(&hare_id);
    // uv_thread_join(&tortoise_id);
    uv_barrier_wait(&blocker);
    uv_barrier_destroy(&blocker);
    // for(;;) {
    //     sleep(1);
    // }
    return 0;
}
