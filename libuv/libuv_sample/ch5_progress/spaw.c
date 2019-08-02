#include <stdio.h>
#include <inttypes.h>

#include <uv.h>

uv_loop_t *loop;
uv_process_t child_req;
uv_process_options_t options;

// #define BARRIER
#ifdef BARRIER
uv_barrier_t barrier;
#endif

void on_exit(uv_process_t *req, int64_t exit_status, int term_signal) {
    fprintf(stderr, "Process exited with status %" PRId64 ", signal %d\n", exit_status, term_signal);
#ifdef BARRIER
    if(uv_barrier_wait(&barrier) > 0) 
        uv_barrier_destroy(&barrier);
#endif
    // uv_close((uv_handle_t*) req, NULL);
}

int main() {
    loop = uv_default_loop();

    char* args[3];
    // args[0] = "mkdir";
    args[0] = "touch";
    args[1] = "test-dir";
    args[2] = NULL;

    options.exit_cb = on_exit;
    options.file = "mkdir";
    options.args = args;
    // the configure working directory
    // options.cwd = "../";
#ifdef BARRIER
    uv_barrier_init(&barrier, 2);
#endif
    int r;
    if ((r = uv_spawn(loop, &child_req, &options))) {
        fprintf(stderr, "%s\n", uv_strerror(r));
        return 1;
    } else {
        fprintf(stderr, "Launched process with ID %d\n", child_req.pid);
    }

    uv_run(loop, UV_RUN_DEFAULT);
#ifdef BARRIER
    uv_barrier_wait(&barrier);
    uv_barrier_destroy(&barrier);
#endif
    return 0;
}
