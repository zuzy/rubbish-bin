#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <uv.h>

uv_loop_t *loop;
uv_udp_t send_socket;
uv_udp_t recv_socket;

typedef struct {
    uv_udp_send_t req;
    uv_buf_t *buf;
} udp_send_obj_t;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;
}

uv_buf_t *make_discover_msg() {
    uv_buf_t *buffer = malloc(sizeof(uv_buf_t));
    // alloc_buffer(NULL, 1024, buffer);
    memset(buffer->base, 0, buffer->len);
    
    char ss[] = "{\"cmd\":\"hopediscover\",\"params\":{\"hopeid\":\"e0:76:d0:92:87:c2\",\"hopeport\":7000,\"hopeip\":\"192.168.31.67\",\"versioncode\": 101,\"versionname\": \"1.0.1\",\"haloversion\": 101,\"model\": \"HOPE-Q3\",\"device\": \"AUDIO-A10-ORV-AL\",\"targetid\": \"e076d092265b\",\"power\": 1}}";
    buffer->base = strdup(ss);
    buffer->len = strlen(ss);
    // strncpy(buffer->base, ss, strlen(ss));
    printf("buf is %s\n", buffer->base);
    sleep(1);
    return buffer;
}

void on_send(uv_udp_send_t *req, int status) {
    udp_send_obj_t *obj = (udp_send_obj_t *)req;
    if(obj->buf) {
        free(obj->buf->base);
    }
    if (status) {
        fprintf(stderr, "Send error %s\n", uv_strerror(status));
        return;
    }
}

void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
    if (nread < 0) {
        fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) req, NULL);
        free(buf->base);
        return;
    }

    char sender[17] = { 0 };
    uv_ip4_name((const struct sockaddr_in*) addr, sender, 16);
    fprintf(stderr, "Recv from %s %s\n", sender, buf->base);
    
    // uv_udp_send_t send_req;
    uv_buf_t *discover_msg = make_discover_msg();
    udp_send_obj_t send_req = {
        .buf = discover_msg,
    };

    struct sockaddr_in send_addr;
    uv_ip4_addr("224.0.0.1", 19602, &send_addr);
    uv_udp_send((uv_udp_send_t *)&send_req, &send_socket, discover_msg, 1, (const struct sockaddr *)&send_addr, on_send);

    // // ... DHCP specific code
    // unsigned int *as_integer = (unsigned int*)buf->base;
    // unsigned int ipbin = ntohl(as_integer[4]);
    // unsigned char ip[4] = {0};
    // int i;
    // for (i = 0; i < 4; i++)
    //     ip[i] = (ipbin >> i*8) & 0xff;
    // fprintf(stderr, "Offered IP %d.%d.%d.%d\n", ip[3], ip[2], ip[1], ip[0]);

    free(buf->base);
    uv_udp_recv_stop(req);
}

int main() {
    loop = uv_default_loop();

    uv_udp_init(loop, &recv_socket);
    struct sockaddr_in recv_addr;
    // uv_ip4_addr("0.0.0.0", 68, &recv_addr);
    uv_ip4_addr("224.0.0.1", 19601, &recv_addr);
    uv_udp_bind(&recv_socket, (const struct sockaddr *)&recv_addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&recv_socket, alloc_buffer, on_read);

    uv_udp_init(loop, &send_socket);
    struct sockaddr_in broadcast_addr;
    uv_ip4_addr("0.0.0.0", 0, &broadcast_addr);
    uv_udp_bind(&send_socket, (const struct sockaddr *)&broadcast_addr, 0);
    uv_udp_set_broadcast(&send_socket, 1);

     // uv_udp_bind(&send_socket, (const struct sockaddr *)&group_addr, 0);
    // uv_udp_set_broadcast(&send_socket, 1);



    return uv_run(loop, UV_RUN_DEFAULT);
}
