#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <stddef.h>

#ifndef container_of
#define container_of(ptr, type, member)					\
	({								\
		const __typeof__(((type *) NULL)->member) *__mptr = (ptr);	\
		(type *) ((char *) __mptr - offsetof(type, member));	\
	})
#endif

#define DEFAULT_PORT 7000
#define DEFAULT_BACKLOG 128

uv_loop_t *loop;
struct sockaddr_in addr;

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

typedef struct {
    uv_tcp_t connector;
    int id;
    char *ip;
    int port;
} connect_obj_t;

void free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}

void echo_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    free_write_req(req);
}

void echo_read(uv_stream_t *steam, ssize_t nread, const uv_buf_t *buf) {

    connect_obj_t *client = (connect_obj_t*)container_of(steam, connect_obj_t, connector);
    // connect_obj_t *client = (connect_obj_t*)steam;

    if (nread > 0) {
        write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
        req->buf = uv_buf_init(buf->base, nread);
        uv_write((uv_write_t*) req, steam, &req->buf, 1, echo_write);
        printf("recv from %d %s %d: %s", client->id, client->ip, client->port, buf->base);
        return;
    }
    if (nread < 0) {
        printf("end!!!\n");
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) steam, NULL);
    }

    free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }


    static int index = 0;

    connect_obj_t *client = (connect_obj_t*) malloc(sizeof(connect_obj_t));
    client->id = ++index;
    // client->connector = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    printf("connector build %p %p %d\n", client, &client->connector, client->id);
    
    uv_tcp_init(loop, &client->connector);
    if (uv_accept(server, (uv_stream_t*) &client->connector) == 0) {
        struct sockaddr_in addr;
        int len;
        uv_tcp_getpeername(&client->connector, (struct sockaddr *)&addr, &len);
        printf("get connection %s %d\n", inet_ntoa(addr.sin_addr), htons(addr.sin_port));

        client->ip = strdup(inet_ntoa(addr.sin_addr));
        client->port = htons(addr.sin_port);

        uv_read_start((uv_stream_t*) &client->connector, alloc_buffer, echo_read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

int main() {
    loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    // uv_ip4_addr("0.0.0.0", 0, &addr);
    
    uv_ip4_addr("0.0.0.0", 7000, &addr);

    char name[1024] = "\0";
    uv_ip4_name(&addr, name, sizeof(name));
    // printf("addr %s\n", name);

    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);

    int r = uv_listen((uv_stream_t*) &server, DEFAULT_BACKLOG, on_new_connection);
    // int len;
    // struct sockaddr_in ad;
    // getsockname((int)server.io_watcher.fd, (struct sockaddr *)&ad, &len);
    // uv_tcp_getsockname(&server, (struct sockaddr *)&ad, &len);
    // printf("bind %s %d\n", inet_ntoa(ad.sin_addr), htons(addr.sin_port));

    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}
