#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

uv_loop_t *loop;
#define PIPE_PATH       "/tmp/broadcast.sock"

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

typedef struct _uv_stream_list_t{
    struct _uv_stream_list_t *prev;
    struct _uv_stream_list_t *next;
    uv_stream_t *stream;
    int size;
    uv_buf_t buf;
} uv_stream_list_t;

static uv_stream_list_t head = {
    .prev = &head,
    .next = &head,
    .stream = NULL,
    .size = 0,
};

static int _add_stream_to_list(uv_stream_t *stream, uv_stream_list_t *head)
{
    uv_stream_list_t *tmp = malloc(sizeof(uv_stream_list_t));
    if(!tmp) return -1;
    tmp->next = head;
    tmp->prev = head->prev;
    tmp->stream = stream;
    head->prev->next = tmp;
    head->prev = tmp;
    ++head->size;
    return 0;
}

static int _delete_stream_from_list(uv_stream_t *stream, uv_stream_list_t *head)
{
    uv_stream_list_t *ptr = head;
    do {
        if(ptr->stream == stream) {
            goto Find;
        }
        ptr = ptr->next;
    } while(ptr != head);
    uv_close((uv_handle_t*)stream, NULL);
    return 0;
    Find: {
        --head->size;
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        uv_close((uv_handle_t*) ptr->stream, NULL);
        free(ptr);
    }
    return 0;
}

void free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
}

void echo_write_final(uv_write_t *req, int status) {
    if (status < 0) {
        fprintf(stderr, "Write error %s\n", uv_err_name(status));
    }
    free_write_req(req);
}

void echo_write(uv_write_t *req, int status) {
    if(status < 0) {
        fprintf(stderr, "write error %s\n", uv_err_name(status));
    }
    free(req);
}

// static void count_write(uv_write_t *req, int status) {
//     if (status < 0) {
//         fprintf(stderr, "Write error %s\n", uv_err_name(status));
//     }
// }

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        uv_stream_list_t *ptr = head.next;
        while(ptr != head.prev) {
            write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
            req->buf = uv_buf_init(buf->base, nread);
            printf("write %s", req->buf.base);
            uv_write((uv_write_t*) req, ptr->stream, &req->buf, 1, echo_write);
            ptr = ptr->next;
        }
        if(ptr != &head) {
            write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
            req->buf = uv_buf_init(buf->base, nread);
            printf("write final %s", req->buf.base);
            uv_write((uv_write_t*) req, ptr->stream, &req->buf, 1, echo_write_final);
        }
        return;
    }

    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        printf("delete!!\n");
        _delete_stream_from_list(client, &head);
        // uv_close((uv_handle_t*) client, NULL);
    }

    free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status == -1) {
        // error!
        return;
    }

    uv_pipe_t *client = (uv_pipe_t*) malloc(sizeof(uv_pipe_t));
    uv_pipe_init(loop, client, 0);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        _add_stream_to_list((uv_stream_t *)client, &head);
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

void remove_sock(int sig) {
    uv_fs_t req;
    uv_fs_unlink(loop, &req, PIPE_PATH, NULL);
    exit(0);
}

int main() {
    loop = uv_default_loop();

    uv_pipe_t server;
    uv_pipe_init(loop, &server, 0);

    signal(SIGINT, remove_sock);

    int r;
    if ((r = uv_pipe_bind(&server, PIPE_PATH))) {
        fprintf(stderr, "Bind error %s\n", uv_err_name(r));
        return 1;
    }
    if ((r = uv_listen((uv_stream_t*) &server, 128, on_new_connection))) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(r));
        return 2;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}
